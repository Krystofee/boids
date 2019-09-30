#include <stdlib.h>
#include <time.h>
#include <simple2d.h>

#define TRUE (1)
#define FALSE (0)

#define FOR(x, n) for (int x = 0; x < n; ++x)

#define TITLE ("Boids")
#define WIDTH (760)
#define HEIGHT (640)
#define FPS_CAP (60)

#define WHITE 180, 180, 255, 1

#define BOIDS_COUNT (500)
#define BOID_RADIUS (3)
#define BOID_SECTORS (3)
#define BOID_SPEED (1)
#define BOID_SPEED_VARIANT (3)
#define BOID_STEERING_COEFICIENT (0.01)
#define REACTIVE_RADIUS (20)

#define FOLLOW_MOUSE FALSE
#define RENDER_TEXT FALSE
#define RENDER_REACT_BOX FALSE

S2D_Window *window;

int stop = FALSE;

typedef struct boid_struct
{
    double x, y;
    double speed;
    double angle;
} boid_t;

boid_t boids[BOIDS_COUNT];
boid_t *working_array[BOIDS_COUNT];

void init()
{
    srand(time(NULL));

    FOR(i, BOIDS_COUNT)
    {
        boid_t boid = {
            rand() % WIDTH,
            rand() % HEIGHT,
            (rand() % (int)(BOID_SPEED_VARIANT * 100)) / 100 + BOID_SPEED,
            fmod(rand(), 2 * M_PI)};

        printf("Boid %d [%f %f] %f\n", i, boid.x, boid.y, boid.angle);

        boids[i] = boid;
    }
}

double distance(double x1, double y1, double x2, double y2)
{
    return hypot(x2 - x1, y2 - y1);
}

void get_closest_boids(boid_t *boid, double radius, size_t *count)
{
    *count = 0;
    FOR(i, BOIDS_COUNT)
    {
        if (&boids[i] == boid)
            continue;

        if (distance(boid->x, boid->y, boids[i].x, boids[i].y) <= radius)
        {
            working_array[*count] = &boids[i];
            *count += 1;
        }
    }
}

void move_boid(boid_t *boid)
{
    size_t n;
    get_closest_boids(boid, REACTIVE_RADIUS, &n);

    if (n)
    {
        double angle = working_array[0]->angle;
        int i;
        for (i = 1; i < n; ++i)
        {
            angle += working_array[i]->angle;
        }
        boid->angle = angle / n;
    }

    // calculate next point based on current location and angle
    double nx = 0,
           ny = 0;
    nx = boid->x + boid->speed * cos(boid->angle);
    ny = boid->y + boid->speed * sin(boid->angle);

    if (nx < 0)
        nx = WIDTH;

    if (nx > WIDTH)
        nx = 0;

    if (ny < 0)
        ny = HEIGHT;

    if (ny > HEIGHT)
        ny = 0;

    boid->x = nx;
    boid->y = ny;
}

void update()
{
    if (stop == TRUE)
        return;

    FOR(i, BOIDS_COUNT)
    {
        move_boid(&boids[i]);
    }
}

void render_boid(boid_t *boid)
{
    S2D_DrawCircle(boid->x, boid->y, BOID_RADIUS, BOID_SECTORS, WHITE);

    if (RENDER_REACT_BOX)
        S2D_DrawCircle(boid->x, boid->y, REACTIVE_RADIUS / 2, 36, 255, 255, 255, 0.1);

    if (RENDER_TEXT)
    {
        char *format_str = "[%.2f,%.2f] %.2f";

        char *buf;
        size_t text_length;
        text_length = snprintf(NULL, 0, format_str, boid->x, boid->y, boid->angle) + 1;
        buf = (char *)malloc(text_length);
        snprintf(buf, text_length, format_str, boid->x, boid->y, boid->angle);

        S2D_Text *txt = S2D_CreateText("vera.ttf", buf, 13);
        if (txt != NULL)
        {
            txt->x = boid->x;
            txt->y = boid->y + 2;
            S2D_DrawText(txt);
            S2D_FreeText(txt);
        }
    }
}

void render()
{
    FOR(i, BOIDS_COUNT)
    {
        render_boid(&boids[i]);
    }
}

int main()
{
    init();

    window = S2D_CreateWindow(TITLE, WIDTH, HEIGHT, update, render, S2D_HIGHDPI);
    window->fps_cap = FPS_CAP;

    S2D_Show(window);
    return 0;
}
