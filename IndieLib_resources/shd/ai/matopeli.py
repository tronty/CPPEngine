// ChatGPT-o1-preview: voisitko luoda matopelin python kielellä?

import pygame
import time
import random

# Alustetaan pygame
pygame.init()

# Varit
valkoinen = (255, 255, 255)
keltainen = (255, 255, 102)
musta = (0, 0, 0)
punainen = (213, 50, 80)
vihrea = (0, 255, 0)
sininen = (50, 153, 213)

# Ikkunan koko
leveys = 600
korkeus = 400

# Pygame-ikkuna
dis = pygame.display.set_mode((leveys, korkeus))
pygame.display.set_caption('Matopeli')

# Ajan hallinta
kello = pygame.time.Clock()

# Mato
mato_koko = 10
mato_nopeus = 15

# Fontit
fontti = pygame.font.SysFont("bahnschrift", 25)

def pisteet(score):
    value = fontti.render("Pisteet: " + str(score), True, keltainen)
    dis.blit(value, [0, 0])

def mato(mato_koko, maton_palikat):
    for x in maton_palikat:
        pygame.draw.rect(dis, musta, [x[0], x[1], mato_koko, mato_koko])

def viesti(msg, vari):
    mesg = fontti.render(msg, True, vari)
    dis.blit(mesg, [leveys / 6, korkeus / 3])

def peli():
    peli_paattyi = False
    peli_suljettu = False

    x = leveys / 2
    y = korkeus / 2

    x_nopeus = 0
    y_nopeus = 0

    maton_palikat = []
    maton_pituus = 1

    ruoka_x = round(random.randrange(0, leveys - mato_koko) / 10.0) * 10.0
    ruoka_y = round(random.randrange(0, korkeus - mato_koko) / 10.0) * 10.0

    while not peli_paattyi:

        while peli_suljettu == True:
            dis.fill(sininen)
            viesti("Havisit! Paina Q lopettaaksesi tai C jatkaaksesi", punainen)
            pisteet(maton_pituus - 1)
            pygame.display.update()

            for event in pygame.event.get():
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_q:
                        peli_paattyi = True
                        peli_suljettu = False
                    if event.key == pygame.K_c:
                        peli()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                peli_paattyi = True
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    x_nopeus = -mato_koko
                    y_nopeus = 0
                elif event.key == pygame.K_RIGHT:
                    x_nopeus = mato_koko
                    y_nopeus = 0
                elif event.key == pygame.K_UP:
                    y_nopeus = -mato_koko
                    x_nopeus = 0
                elif event.key == pygame.K_DOWN:
                    y_nopeus = mato_koko
                    x_nopeus = 0

        if x >= leveys or x < 0 or y >= korkeus or y < 0:
            peli_suljettu = True
        x += x_nopeus
        y += y_nopeus
        dis.fill(sininen)
        pygame.draw.rect(dis, vihrea, [ruoka_x, ruoka_y, mato_koko, mato_koko])

        maton_paa = []
        maton_paa.append(x)
        maton_paa.append(y)
        maton_palikat.append(maton_paa)
        if len(maton_palikat) > maton_pituus:
            del maton_palikat[0]

        for segment in maton_palikat[:-1]:
            if segment == maton_paa:
                peli_suljettu = True

        mato(mato_koko, maton_palikat)
        pisteet(maton_pituus - 1)

        pygame.display.update()

        if x == ruoka_x and y == ruoka_y:
            ruoka_x = round(random.randrange(0, leveys - mato_koko) / 10.0) * 10.0
            ruoka_y = round(random.randrange(0, korkeus - mato_koko) / 10.0) * 10.0
            maton_pituus += 1

        kello.tick(mato_nopeus)

    pygame.quit()
    quit()

peli()

