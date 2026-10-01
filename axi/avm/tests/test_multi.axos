state user_buffer: array[UserStruct, 500] = @Zero_Init

struct DashboardState:
    public title: string[64]
    public theme_color: string[7]
    public active_users: int32

struct FilterParams:
    public is_active: bool
    public role: string[32]

struct DroneTelemetry:
    public drone_id: string[36]
    public altitude: float32
    public velocity: float32

node fetch_active_users(limit: int32) -> array[UserStruct, 500]:
    @SQL(dialect="postgresql", version="18.4", async_io=True)
    """
    SELECT id, username, clearance_level 
    FROM ethos_personnel 
    WHERE status = 'active' 
    ORDER BY created_at DESC 
    LIMIT {limit};
    """

node dynamic_user_query(filters: FilterParams) -> array[UserStruct, 250]:
    @SQLAlchemy
    """
    from sqlalchemy import select
    from ethos.models import User
    
    stmt = select(User)
    
    if filters.is_active:
        stmt = stmt.where(User.status == 'active')
        
    if filters.role:
        stmt = stmt.where(User.role == filters.role)
        
    return session.scalars(stmt).fetchmany(250)
    """

node render_card(state: DashboardState) -> string[2048]:
    @Stylus
    """
    primary-bg = {state.theme_color}
    
    .ethos-card
      background: primary-bg
    """
    
    @HTML
    """
    <div class="ethos-card">
        <h1>{state.title}</h1>
        <div class="metric-readout">Users: {state.active_users}</div>
    </div>
    """

node get_drone_status(request_id: string[36]) -> DroneTelemetry:
    @GraphQL(type="Query", field="droneStatus")
    
    request_id -> fetch_hardware_sensor -> telemetry_data
