#ifndef __APP_H__
#define __APP_H__

/*
 * main application class
 *
 * no instance of this is ever really created, just used for
 * putting everything in one place
 */

class App
{
    protected:
        static void Init(int *argc, char **argv);
        static void Loop(void);
        static void Quit(void);
        static int ParseArgs(int argc, char **argv);

        // GLUT callbacks

        static void Idle(void);
        static void WindowResized(int w, int h);

        static void Draw(void);

        static void MouseMoved(int x, int y);
        static void MouseClicked(int button, int state, int x, int y);

        static void KeyPressed(unsigned char key, int x, int y);
        static void KeyPressedSpecial(int key, int x, int y);


    public:
        static void Run(int *argc, char **argv)
        {
            Init(argc, argv);
            Loop();
        }

};

#endif
