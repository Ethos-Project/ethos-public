#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#define AX_PATH_CAP 32768
#define AX_SOURCE_CAP 1048576
#define AX_NODE_CAP 256
#define AX_NAME_CAP 96

enum { AX_OK=0, AX_USAGE=2, AX_INPUT=3, AX_PIPELINE=4, AX_BACKEND=5, AX_ARTIFACT=6 };

typedef struct {
    char names[AX_NODE_CAP][AX_NAME_CAP];
    int edges[AX_NODE_CAP][AX_NODE_CAP];
    int edge_count[AX_NODE_CAP];
    int indegree[AX_NODE_CAP];
    int count;
} AxGraph;

static int join_path(wchar_t out[AX_PATH_CAP],const wchar_t*left,const wchar_t*right){
    int n=_snwprintf(out,AX_PATH_CAP,L"%ls\\%ls",left,right);return n>=0&&n<AX_PATH_CAP;
}
static int regular_file(const wchar_t*path){DWORD a=GetFileAttributesW(path);return a!=INVALID_FILE_ATTRIBUTES&&!(a&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT));}
static int full_path(const wchar_t*in,wchar_t out[AX_PATH_CAP]){DWORD n=GetFullPathNameW(in,AX_PATH_CAP,out,NULL);return n>0&&n<AX_PATH_CAP;}
static int is_installation_root(const wchar_t*root){wchar_t tool[AX_PATH_CAP];return join_path(tool,root,L"bootstrap\\python_to_c_compiler\\bin\\mingw64\\bin\\gcc.exe")&&regular_file(tool);}
static int installation_root(wchar_t root[AX_PATH_CAP]){
    DWORD n=GetModuleFileNameW(NULL,root,AX_PATH_CAP);wchar_t*s,candidate[AX_PATH_CAP];
    if(!n||n>=AX_PATH_CAP||!(s=wcsrchr(root,L'\\')))return 0;
    *s=0;
    if(is_installation_root(root))return 1;
    if(GetCurrentDirectoryW(AX_PATH_CAP,candidate)&&join_path(root,candidate,L"lang")&&is_installation_root(root))return 1;
    return 0;
}
static int build_command_line(const wchar_t*const argv[],wchar_t command[AX_PATH_CAP]){
    size_t used=0,i;
    for(i=0;argv[i];++i){size_t n=wcslen(argv[i]);if(wcschr(argv[i],L'"')||used+n+4>=AX_PATH_CAP)return 0;if(used)command[used++]=L' ';command[used++]=L'"';memcpy(command+used,argv[i],n*sizeof(wchar_t));used+=n;command[used++]=L'"';}
    command[used]=0;return 1;
}
static int run_process(const wchar_t*tool,const wchar_t*const argv[]){
    STARTUPINFOW startup;PROCESS_INFORMATION process;wchar_t command[AX_PATH_CAP];DWORD exit_code;
    if(!regular_file(tool)){fwprintf(stderr,L"[Axi] backend missing: %ls\n",tool);return AX_BACKEND;}
    if(!build_command_line(argv,command))return AX_BACKEND;
    memset(&startup,0,sizeof(startup));startup.cb=sizeof(startup);memset(&process,0,sizeof(process));
    if(!CreateProcessW(tool,command,NULL,NULL,FALSE,0,NULL,NULL,&startup,&process)){fwprintf(stderr,L"[Axi] backend launch failed error=%lu: %ls\n",(unsigned long)GetLastError(),tool);return AX_BACKEND;}
    WaitForSingleObject(process.hProcess,INFINITE);
    if(!GetExitCodeProcess(process.hProcess,&exit_code)){CloseHandle(process.hThread);CloseHandle(process.hProcess);return AX_BACKEND;}
    CloseHandle(process.hThread);CloseHandle(process.hProcess);
    if(exit_code){fwprintf(stderr,L"[Axi] backend exit=%lu: %ls\n",(unsigned long)exit_code,tool);return AX_BACKEND;}return AX_OK;
}

static int compile_native(const wchar_t*root,const wchar_t*input,const wchar_t*output,int cpp){
    wchar_t tool[AX_PATH_CAP];const wchar_t*args[13];int code,index=0;
    const wchar_t*rel=cpp?L"bootstrap\\python_to_c_compiler\\bin\\mingw64\\bin\\g++.exe":L"bootstrap\\python_to_c_compiler\\bin\\mingw64\\bin\\gcc.exe";
    if(!join_path(tool,root,rel))return AX_BACKEND;
    DeleteFileW(output);
    args[index++]=tool;args[index++]=cpp?L"-std=c++20":L"-std=c11";args[index++]=L"-Wall";args[index++]=L"-Wextra";args[index++]=L"-Werror";args[index++]=L"-pedantic";
    if(cpp){args[index++]=L"-static-libstdc++";args[index++]=L"-static-libgcc";}
    args[index++]=input;args[index++]=L"-o";args[index++]=output;args[index]=NULL;
    code=run_process(tool,args);if(code)return code;return regular_file(output)?AX_OK:AX_ARTIFACT;
}

static char*read_source(const wchar_t*path,size_t*length){FILE*f=_wfopen(path,L"rb");char*s;long n;if(!f||fseek(f,0,SEEK_END)||(n=ftell(f))<0||n>AX_SOURCE_CAP||fseek(f,0,SEEK_SET)){if(f)fclose(f);return NULL;}s=(char*)malloc((size_t)n+1);if(!s||fread(s,1,(size_t)n,f)!=(size_t)n){free(s);fclose(f);return NULL;}fclose(f);s[n]=0;*length=(size_t)n;return s;}
static int graph_index(AxGraph*g,const char*name){int i;for(i=0;i<g->count;++i)if(!strcmp(g->names[i],name))return i;if(g->count>=AX_NODE_CAP||!name[0]||strlen(name)>=AX_NAME_CAP)return-1;strcpy(g->names[g->count],name);return g->count++;}
static int parse_graph(const char*source,AxGraph*g){const char*c=source;int native=0;memset(g,0,sizeof(*g));while(*c){const char*e=strchr(c,'\n');size_t n=e?(size_t)(e-c):strlen(c);char line[1024],a[AX_NAME_CAP],b[AX_NAME_CAP];int x,y;if(n>=sizeof(line))return 0;memcpy(line,c,n);line[n]=0;if(strstr(line,"\"\"\""))native=!native;if(!native&&line[0]!='#'&&strstr(line,"->")&&sscanf(line," %95s -> %95s",a,b)==2){x=graph_index(g,a);y=graph_index(g,b);if(x<0||y<0||g->edge_count[x]>=AX_NODE_CAP)return 0;g->edges[x][g->edge_count[x]++]=y;g->indegree[y]++;}c=e?e+1:c+n;}return g->count>0;}

static int emit_axi_c(const wchar_t*input,const wchar_t*generated){
    size_t ignored,i;char*source=read_source(input,&ignored),*cursor;AxGraph g;FILE*out;int q[AX_NODE_CAP],order[AX_NODE_CAP],front=0,back=0,count=0;
    (void)ignored;if(!source||!parse_graph(source,&g)){free(source);fwprintf(stderr,L"[Axi] .axi graph parse failed\n");return AX_PIPELINE;}
    for(i=0;i<(size_t)g.count;++i){
        if(!g.indegree[i])q[back++]=(int)i;
    }
    while(front<back){int n=q[front++],j;order[count++]=n;for(j=0;j<g.edge_count[n];++j){int t=g.edges[n][j];if(!--g.indegree[t])q[back++]=t;}}
    if(count!=g.count){free(source);return AX_PIPELINE;}out=_wfopen(generated,L"wb");if(!out){free(source);return AX_ARTIFACT;}
    fputs("#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdbool.h>\n",out);cursor=source;
    while((cursor=strstr(cursor,"@C_Include"))){char inc[256];if(sscanf(cursor,"@C_Include(\"%255[^\"]\")",inc)==1)fprintf(out,"#include %s\n",inc);cursor+=10;}
    fputs("int g_argc; char **g_argv;\n",out);cursor=source;
    while((cursor=strstr(cursor,"node "))){char name[AX_NAME_CAP]={0};char*start,*end;if(sscanf(cursor,"node %95[^ (]",name)!=1||!(start=strstr(cursor,"\"\"\""))||!(end=strstr(start+3,"\"\"\""))){fclose(out);free(source);DeleteFileW(generated);return AX_PIPELINE;}fprintf(out,"static void %s(void){\n",name);fwrite(start+3,1,(size_t)(end-start-3),out);fputs("\n}\n",out);cursor=end+3;}
    fputs("int main(int argc,char**argv){g_argc=argc;g_argv=argv;\n",out);for(i=0;i<(size_t)count;++i){char find[AX_NAME_CAP+8];snprintf(find,sizeof(find),"node %s",g.names[order[i]]);if(strstr(source,find))fprintf(out,"%s();\n",g.names[order[i]]);}fputs("return 0;}\n",out);
    if(fclose(out)){free(source);DeleteFileW(generated);return AX_ARTIFACT;}free(source);return regular_file(generated)?AX_OK:AX_ARTIFACT;
}
static int compile_axi(const wchar_t*root,const wchar_t*input,const wchar_t*output){wchar_t generated[AX_PATH_CAP];int n=_snwprintf(generated,AX_PATH_CAP,L"%ls.c",output),code;if(n<0||n>=AX_PATH_CAP)return AX_ARTIFACT;DeleteFileW(generated);code=emit_axi_c(input,generated);if(code)return code;code=compile_native(root,generated,output,0);if(!code)wprintf(L"[Axi] generated C: %ls\n",generated);return code;}

static int wide_to_utf8(const wchar_t*in,char*out,size_t cap){return cap<=INT_MAX&&WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,in,-1,out,(int)cap,NULL,NULL)>0;}
static int write_utf8(const wchar_t*path,const char*text){FILE*f=_wfopen(path,L"wb");size_t n=strlen(text);int ok;if(!f)return 0;ok=fwrite(text,1,n,f)==n&&fflush(f)==0;ok=fclose(f)==0&&ok;if(!ok)DeleteFileW(path);return ok;}
static int output_parts(const wchar_t*output,wchar_t dir[AX_PATH_CAP],wchar_t name[AX_NAME_CAP]){wchar_t full[AX_PATH_CAP],*slash,*dot,*p;size_t n;if(!full_path(output,full)||!(slash=wcsrchr(full,L'\\'))||!(dot=wcsrchr(slash+1,L'.'))||_wcsicmp(dot,L".exe"))return 0;n=(size_t)(dot-slash-1);if(!n||n>=AX_NAME_CAP)return 0;wcsncpy(name,slash+1,n);name[n]=0;for(p=name;*p;++p)if(!(iswalnum(*p)||*p==L'_'||*p==L'-'))return 0;*slash=0;wcscpy(dir,full);return 1;}

static int delete_tree(const wchar_t*path){wchar_t pattern[AX_PATH_CAP],child[AX_PATH_CAP];WIN32_FIND_DATAW entry;HANDLE find;DWORD a=GetFileAttributesW(path);int ok=1;if(a==INVALID_FILE_ATTRIBUTES)return 1;if(!(a&FILE_ATTRIBUTE_DIRECTORY)||(a&FILE_ATTRIBUTE_REPARSE_POINT))return 0;if(!join_path(pattern,path,L"*"))return 0;find=FindFirstFileW(pattern,&entry);if(find!=INVALID_HANDLE_VALUE){do{if(!wcscmp(entry.cFileName,L".")||!wcscmp(entry.cFileName,L".."))continue;if(!join_path(child,path,entry.cFileName)){ok=0;break;}if(entry.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY){if(entry.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT){ok=0;break;}if(!delete_tree(child)){ok=0;break;}}else if(!DeleteFileW(child)){ok=0;break;}}while(FindNextFileW(find,&entry));FindClose(find);}return ok&&RemoveDirectoryW(path);}

static int compile_csharp(const wchar_t*root,const wchar_t*input,const wchar_t*output){
    wchar_t dotnet[AX_PATH_CAP],tmpbase[AX_PATH_CAP],tmp[AX_PATH_CAP],project[AX_PATH_CAP],nuget_dir[AX_PATH_CAP],config[AX_PATH_CAP],packages[AX_PATH_CAP],dir[AX_PATH_CAP],name[AX_NAME_CAP],fullinput[AX_PATH_CAP],check[AX_PATH_CAP];
    wchar_t saved_appdata[AX_PATH_CAP]={0},saved_dotnet_home[AX_PATH_CAP]={0},saved_skip_first[16]={0},saved_telemetry[16]={0},saved_nologo[16]={0},saved_certificate[16]={0},saved_tools_path[16]={0};
    const wchar_t*current_appdata=_wgetenv(L"APPDATA"),*current_dotnet_home=_wgetenv(L"DOTNET_CLI_HOME"),*current_skip_first=_wgetenv(L"DOTNET_SKIP_FIRST_TIME_EXPERIENCE"),*current_telemetry=_wgetenv(L"DOTNET_CLI_TELEMETRY_OPTOUT"),*current_nologo=_wgetenv(L"DOTNET_NOLOGO"),*current_certificate=_wgetenv(L"DOTNET_GENERATE_ASPNET_CERTIFICATE"),*current_tools_path=_wgetenv(L"DOTNET_ADD_GLOBAL_TOOLS_TO_PATH");
    char input8[AX_PATH_CAP],name8[AX_NAME_CAP],xml[AX_PATH_CAP];const wchar_t*restore_args[10];const wchar_t*build_args[12];DWORD tn;int n,code;unsigned long nonce=(unsigned long)(GetTickCount64()^GetCurrentProcessId());
    if(!join_path(dotnet,root,L"bootstrap\\python_to_c_compiler\\bin\\dotnet\\dotnet.exe")||!output_parts(output,dir,name)||!full_path(input,fullinput)||!wide_to_utf8(fullinput,input8,sizeof(input8))||!wide_to_utf8(name,name8,sizeof(name8))||strpbrk(input8,"&<>\"")!=NULL)return AX_INPUT;
    if(current_appdata)wcsncpy(saved_appdata,current_appdata,AX_PATH_CAP-1);
    if(current_dotnet_home)wcsncpy(saved_dotnet_home,current_dotnet_home,AX_PATH_CAP-1);
    if(current_skip_first)wcsncpy(saved_skip_first,current_skip_first,15);
    if(current_telemetry)wcsncpy(saved_telemetry,current_telemetry,15);
    if(current_nologo)wcsncpy(saved_nologo,current_nologo,15);
    if(current_certificate)wcsncpy(saved_certificate,current_certificate,15);
    if(current_tools_path)wcsncpy(saved_tools_path,current_tools_path,15);
    tn=GetTempPathW(AX_PATH_CAP,tmpbase);n=_snwprintf(tmp,AX_PATH_CAP,L"%lsAxi-cs-%lu",tmpbase,nonce);if(!tn||tn>=AX_PATH_CAP||n<0||n>=AX_PATH_CAP||!CreateDirectoryW(tmp,NULL)||!join_path(project,tmp,L"AxiInput.csproj")||!join_path(nuget_dir,tmp,L"NuGet")||!CreateDirectoryW(nuget_dir,NULL)||!join_path(config,nuget_dir,L"NuGet.Config")||!join_path(packages,tmp,L"packages"))return AX_ARTIFACT;
    n=snprintf(xml,sizeof(xml),"<Project Sdk=\"Microsoft.NET.Sdk\">\n<PropertyGroup><OutputType>Exe</OutputType><TargetFramework>net8.0</TargetFramework><AssemblyName>%s</AssemblyName><EnableDefaultCompileItems>false</EnableDefaultCompileItems><UseAppHost>true</UseAppHost><RestoreIgnoreFailedSources>true</RestoreIgnoreFailedSources></PropertyGroup>\n<ItemGroup><Compile Include=\"%s\" Link=\"Program.cs\" /></ItemGroup>\n</Project>\n",name8,input8);
    if(n<0||(size_t)n>=sizeof(xml)||!write_utf8(project,xml)||!write_utf8(config,"<configuration><packageSources><clear /></packageSources></configuration>\n")){delete_tree(tmp);return AX_ARTIFACT;}
    restore_args[0]=dotnet;restore_args[1]=L"restore";restore_args[2]=project;restore_args[3]=L"--configfile";restore_args[4]=config;restore_args[5]=L"--packages";restore_args[6]=packages;restore_args[7]=L"--nologo";restore_args[8]=NULL;
    if(_wputenv_s(L"APPDATA",tmp)||_wputenv_s(L"DOTNET_CLI_HOME",tmp)||_wputenv_s(L"DOTNET_SKIP_FIRST_TIME_EXPERIENCE",L"1")||_wputenv_s(L"DOTNET_CLI_TELEMETRY_OPTOUT",L"1")||_wputenv_s(L"DOTNET_NOLOGO",L"1")||_wputenv_s(L"DOTNET_GENERATE_ASPNET_CERTIFICATE",L"0")||_wputenv_s(L"DOTNET_ADD_GLOBAL_TOOLS_TO_PATH",L"0")){
        _wputenv_s(L"APPDATA",saved_appdata);_wputenv_s(L"DOTNET_CLI_HOME",saved_dotnet_home);_wputenv_s(L"DOTNET_SKIP_FIRST_TIME_EXPERIENCE",saved_skip_first);_wputenv_s(L"DOTNET_CLI_TELEMETRY_OPTOUT",saved_telemetry);_wputenv_s(L"DOTNET_NOLOGO",saved_nologo);_wputenv_s(L"DOTNET_GENERATE_ASPNET_CERTIFICATE",saved_certificate);_wputenv_s(L"DOTNET_ADD_GLOBAL_TOOLS_TO_PATH",saved_tools_path);delete_tree(tmp);return AX_BACKEND;
    }
    code=run_process(dotnet,restore_args);
    if(!code){DeleteFileW(output);build_args[0]=dotnet;build_args[1]=L"build";build_args[2]=project;build_args[3]=L"--no-restore";build_args[4]=L"-c";build_args[5]=L"Release";build_args[6]=L"-o";build_args[7]=dir;build_args[8]=L"--nologo";build_args[9]=L"--verbosity";build_args[10]=L"quiet";build_args[11]=NULL;code=run_process(dotnet,build_args);}
    _wputenv_s(L"APPDATA",saved_appdata);
    _wputenv_s(L"DOTNET_CLI_HOME",saved_dotnet_home);
    _wputenv_s(L"DOTNET_SKIP_FIRST_TIME_EXPERIENCE",saved_skip_first);
    _wputenv_s(L"DOTNET_CLI_TELEMETRY_OPTOUT",saved_telemetry);
    _wputenv_s(L"DOTNET_NOLOGO",saved_nologo);
    _wputenv_s(L"DOTNET_GENERATE_ASPNET_CERTIFICATE",saved_certificate);
    _wputenv_s(L"DOTNET_ADD_GLOBAL_TOOLS_TO_PATH",saved_tools_path);
    if(!delete_tree(tmp)&&!code)code=AX_ARTIFACT;
    if(code)return code;
    if(!regular_file(output))return AX_ARTIFACT;
    n=_snwprintf(check,AX_PATH_CAP,L"%ls\\%ls.dll",dir,name);if(n<0||n>=AX_PATH_CAP||!regular_file(check))return AX_ARTIFACT;n=_snwprintf(check,AX_PATH_CAP,L"%ls\\%ls.runtimeconfig.json",dir,name);if(n<0||n>=AX_PATH_CAP||!regular_file(check))return AX_ARTIFACT;n=_snwprintf(check,AX_PATH_CAP,L"%ls\\%ls.deps.json",dir,name);return n>=0&&n<AX_PATH_CAP&&regular_file(check)?AX_OK:AX_ARTIFACT;
}

static int find_python(wchar_t python[AX_PATH_CAP]){DWORD n=SearchPathW(NULL,L"python.exe",NULL,AX_PATH_CAP,python,NULL);return n>0&&n<AX_PATH_CAP&&regular_file(python);}
static int compile_python(const wchar_t*root,const wchar_t*input,const wchar_t*output){wchar_t python[AX_PATH_CAP],emitter[AX_PATH_CAP],generated[AX_PATH_CAP];const wchar_t*args[5];int n,code;if(!find_python(python)||!join_path(emitter,root,L"src\\pipelines\\python_to_c.py"))return AX_BACKEND;n=_snwprintf(generated,AX_PATH_CAP,L"%ls.py.c",output);if(n<0||n>=AX_PATH_CAP)return AX_ARTIFACT;DeleteFileW(generated);args[0]=python;args[1]=emitter;args[2]=input;args[3]=generated;args[4]=NULL;code=run_process(python,args);if(code)return code;if(!regular_file(generated))return AX_ARTIFACT;return compile_native(root,generated,output,0);}

static void help(void){puts("Axi universal native compiler/transpiler");puts("Usage: Axi_compiler <input.{axi,c,cpp,cc,cxx,cs,py}> <output.exe>");puts("Base Axi uses dedicated pipelines; Psyche shared IR/DAG is an additional layer.");}
int wmain(int argc,wchar_t**argv){
    wchar_t root[AX_PATH_CAP],input[AX_PATH_CAP],output[AX_PATH_CAP];const wchar_t*ext;int code;
    if(argc!=3){help();return AX_USAGE;}
    if(!installation_root(root)||!full_path(argv[1],input)||!full_path(argv[2],output)||!regular_file(input)){fwprintf(stderr,L"[Axi] invalid input or installation\n");return AX_INPUT;}
    ext=wcsrchr(input,L'.');if(!ext)return AX_USAGE;
    DeleteFileW(output);
    if(!_wcsicmp(ext,L".axi"))code=compile_axi(root,input,output);
    else if(!_wcsicmp(ext,L".c"))code=compile_native(root,input,output,0);
    else if(!_wcsicmp(ext,L".cpp")||!_wcsicmp(ext,L".cc")||!_wcsicmp(ext,L".cxx"))code=compile_native(root,input,output,1);
    else if(!_wcsicmp(ext,L".cs"))code=compile_csharp(root,input,output);
    else if(!_wcsicmp(ext,L".py"))code=compile_python(root,input,output);
    else code=AX_USAGE;
    if(code){DeleteFileW(output);fwprintf(stderr,L"[Axi] failed pipeline=%ls code=%d\n",ext,code);return code;}
    wprintf(L"[Axi] verified pipeline=%ls output=%ls\n",ext,output);return AX_OK;
}

