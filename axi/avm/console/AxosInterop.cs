using System;
using System.Runtime.InteropServices;

namespace AVM.UI
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public unsafe struct RenderCommand
    {
        public int Type;
        public float X, Y, W, H;
        public float R, G, B, A;
        public fixed byte Text[64];

        public string GetText()
        {
            fixed (byte* p = Text)
            {
                return Marshal.PtrToStringAnsi((IntPtr)p);
            }
        }
    }

    public static class AxiInterop
    {
        private const string DllName = "axi";

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void axi_init_window(int width, int height);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int axi_render_frame(double deltaTime, [Out] RenderCommand[] commands, int maxCommands);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void axi_handle_input(int key, int state);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void axi_handle_mouse(float x, float y, int button, int state);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void axi_handle_cursor(float x, float y);
    }
}



