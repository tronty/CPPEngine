import org.lwjgl.LWJGLException;
import org.lwjgl.opengl.Display;
import org.lwjgl.opengl.DisplayMode;
import org.lwjgl.opengl.GL11;
import org.lwjgl.input.Keyboard;

public class HelloWorld{
    public void start() {
        try {
            Display.setDisplayMode(new DisplayMode(640, 480));
            Display.create();
        } catch (LWJGLException e) {
            e.printStackTrace();
            System.exit(0);
        }

        // Init OpenGL
        GL11.glMatrixMode(GL11.GL_PROJECTION);
        GL11.glLoadIdentity();
        GL11.glOrtho(-3.2, 3.2, -2.4, 2.4, -1, 1);
        GL11.glMatrixMode(GL11.GL_MODELVIEW);

        boolean quit = false;

        while (!quit) {         
            // Clear the screen.
            GL11.glClear(GL11.GL_COLOR_BUFFER_BIT | GL11.GL_DEPTH_BUFFER_BIT);

            // Begin drawing
            GL11.glBegin(GL11.GL_TRIANGLES);

                // Top & Red
                GL11.glColor3f(1.0f, 0.0f, 0.0f);
                GL11.glVertex2f(0.0f, 1.0f);

                // Right & Green
                GL11.glColor3f(0.0f, 1.0f, 0.0f);
                GL11.glVertex2f(1.0f, 1.0f);

                // Left & Blue
                GL11.glColor3f(0.0f, 0.0f, 1.0f);
                GL11.glVertex2f(1.0f, -1.0f);

            GL11.glEnd();

            Display.update();

            if (Display.isCloseRequested() || Keyboard.isKeyDown(Keyboard.KEY_ESCAPE))
                quit = true;
        }

        Display.destroy();
    }

    public static void main(String args[]) {
        HelloWorld application = new HelloWorld();
        application.start();
    }

}

