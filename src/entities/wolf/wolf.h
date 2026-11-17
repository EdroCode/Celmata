typedef struct WolfList WolfList;

WolfList *create_wolf_pack(int size);
void destroy_wolf_pack(WolfList *list);
void update_wolfs(WolfList *list);
void draw_wolfs(const WolfList *list);
