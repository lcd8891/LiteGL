Какие компоненты игры можно реализовать через litegl-component
# engine_components_create
Функция, вызываемая при запуске движка до начала цикла. Предоставляет возможность, используя класс **engine_components**, создавать event для разных аспектов игры. 
## engine_event_types
Перечисление всех доступных для изменения компонентов движка:
1. `engine_load_begin`
2. `engine_load_end`
3. `engine_on_frame`
4. `engine_mainmenu_open`
5. `engine_world_load_begin`
6. `engine_world_load_end`
7. `engine_arg_process`
8. 
# #LoadState

Обозначат: что в данный момент загружается.
- `Caching` - кэширование данных для повторных загрузок.
- `ConfigPreload` - загрузка настроек игры и движка.
- `ShadersLoading` - загрузка/компиляция шейдеров.
- `SoundsLoading` - загрузка звуков.
- `MusicLoading` - загрузка музыки.
- `TextureLoading` - загрузка текстур.
- `SceneLoading` - загрузка сцены.
- `MapLoading` - загрузка карты.