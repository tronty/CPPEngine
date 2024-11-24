// ChatGPT-o1-preview: Java Pacman?

import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

public class PacmanGame extends JPanel implements KeyListener, Runnable {
    private int pacmanX = 50, pacmanY = 50; // Pacman's position
    private int pacmanSpeed = 5;           // Speed of movement
    private int ghostX = 300, ghostY = 200; // Ghost's position
    private boolean running = true;

    public PacmanGame() {
        JFrame frame = new JFrame("Pacman Game");
        frame.setSize(600, 400);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.add(this);
        frame.addKeyListener(this);
        frame.setVisible(true);
        new Thread(this).start();
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.setColor(Color.BLACK);
        g.fillRect(0, 0, getWidth(), getHeight()); // Background

        // Draw Pacman
        g.setColor(Color.YELLOW);
        g.fillArc(pacmanX, pacmanY, 30, 30, 45, 270);

        // Draw Ghost
        g.setColor(Color.RED);
        g.fillOval(ghostX, ghostY, 30, 30);
    }

    @Override
    public void keyPressed(KeyEvent e) {
        switch (e.getKeyCode()) {
            case KeyEvent.VK_UP -> pacmanY -= pacmanSpeed;
            case KeyEvent.VK_DOWN -> pacmanY += pacmanSpeed;
            case KeyEvent.VK_LEFT -> pacmanX -= pacmanSpeed;
            case KeyEvent.VK_RIGHT -> pacmanX += pacmanSpeed;
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {}

    @Override
    public void keyTyped(KeyEvent e) {}

    @Override
    public void run() {
        while (running) {
            // Ghost follows Pacman
            if (ghostX < pacmanX) ghostX += 2;
            if (ghostX > pacmanX) ghostX -= 2;
            if (ghostY < pacmanY) ghostY += 2;
            if (ghostY > pacmanY) ghostY -= 2;

            // Check collision
            if (new Rectangle(pacmanX, pacmanY, 30, 30).intersects(
                    new Rectangle(ghostX, ghostY, 30, 30))) {
                JOptionPane.showMessageDialog(this, "Game Over!");
                running = false;
                System.exit(0);
            }

            // Repaint the screen
            repaint();

            // Delay for smooth movement
            try {
                Thread.sleep(30);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public static void main(String[] args) {
        new PacmanGame();
    }
}

