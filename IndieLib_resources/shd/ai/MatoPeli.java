// ChatGPT-o1-preview: voisitko luoda matopelin java kielellä?

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.Random;

public class Matopeli extends JFrame {
    private static final int RUUDUN_KOKO = 10;
    private static final int RUUTUJA = 30;
    private static final int PELIN_KOKO = RUUDUN_KOKO * RUUTUJA;
    private static final int ALKUPITUUS = 5;
    private static final int VIIVE = 100;

    private int[] x = new int[RUUTUJA * RUUTUJA];
    private int[] y = new int[RUUTUJA * RUUTUJA];
    private int madonPituus;
    private int ruokaX;
    private int ruokaY;
    private char suunta = 'R';
    private boolean kaynnissa = true;

    private Timer timer;

    public Matopeli() {
        add(new Pelipaneeli());
        setTitle("Matopeli");
        setSize(PELIN_KOKO, PELIN_KOKO);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLocationRelativeTo(null);
        setResizable(false);
    }

    private class Pelipaneeli extends JPanel implements ActionListener {
        public Pelipaneeli() {
            setBackground(Color.BLACK);
            setFocusable(true);
            addKeyListener(new NappaimenKasittelija());
            alustaPeli();
        }

        private void alustaPeli() {
            madonPituus = ALKUPITUUS;
            for (int i = 0; i < madonPituus; i++) {
                x[i] = RUUDUN_KOKO * (madonPituus - i - 1);
                y[i] = 0;
            }
            luoRuoka();
            timer = new Timer(VIIVE, this);
            timer.start();
        }

        private void luoRuoka() {
            Random rand = new Random();
            ruokaX = rand.nextInt(RUUTUJA) * RUUDUN_KOKO;
            ruokaY = rand.nextInt(RUUTUJA) * RUUDUN_KOKO;
        }

        private void liiku() {
            for (int i = madonPituus - 1; i > 0; i--) {
                x[i] = x[i - 1];
                y[i] = y[i - 1];
            }
            switch (suunta) {
                case 'U' -> y[0] -= RUUDUN_KOKO;
                case 'D' -> y[0] += RUUDUN_KOKO;
                case 'L' -> x[0] -= RUUDUN_KOKO;
                case 'R' -> x[0] += RUUDUN_KOKO;
            }
        }

        private void tarkistaTormays() {
            for (int i = madonPituus; i > 0; i--) {
                if ((x[0] == x[i]) && (y[0] == y[i])) {
                    kaynnissa = false;
                }
            }
            if (x[0] < 0 || x[0] >= PELIN_KOKO || y[0] < 0 || y[0] >= PELIN_KOKO) {
                kaynnissa = false;
            }
            if (!kaynnissa) {
                timer.stop();
            }
        }

        private void tarkistaRuoka() {
            if (x[0] == ruokaX && y[0] == ruokaY) {
                madonPituus++;
                luoRuoka();
            }
        }

        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            if (kaynnissa) {
                g.setColor(Color.RED);
                g.fillRect(ruokaX, ruokaY, RUUDUN_KOKO, RUUDUN_KOKO);
                for (int i = 0; i < madonPituus; i++) {
                    if (i == 0) {
                        g.setColor(Color.GREEN);
                    } else {
                        g.setColor(Color.LIGHT_GRAY);
                    }
                    g.fillRect(x[i], y[i], RUUDUN_KOKO, RUUDUN_KOKO);
                }
            } else {
                peliOhi(g);
            }
        }

        private void peliOhi(Graphics g) {
            String viesti = "Peli Ohi!";
            Font fontti = new Font("Helvetica", Font.BOLD, 20);
            FontMetrics met = getFontMetrics(fontti);

            g.setColor(Color.RED);
            g.setFont(fontti);
            g.drawString(viesti, (PELIN_KOKO - met.stringWidth(viesti)) / 2, PELIN_KOKO / 2);
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            if (kaynnissa) {
                tarkistaRuoka();
                liiku();
                tarkistaTormays();
            }
            repaint();
        }

        private class NappaimenKasittelija extends KeyAdapter {
            @Override
            public void keyPressed(KeyEvent e) {
                int key = e.getKeyCode();

                if (key == KeyEvent.VK_LEFT && suunta != 'R') {
                    suunta = 'L';
                }
                if (key == KeyEvent.VK_RIGHT && suunta != 'L') {
                    suunta = 'R';
                }
                if (key == KeyEvent.VK_UP && suunta != 'D') {
                    suunta = 'U';
                }
                if (key == KeyEvent.VK_DOWN && suunta != 'U') {
                    suunta = 'D';
                }
            }
        }
    }

    public static void main(String[] args) {
        EventQueue.invokeLater(() -> {
            JFrame peli = new Matopeli();
            peli.setVisible(true);
        });
        
