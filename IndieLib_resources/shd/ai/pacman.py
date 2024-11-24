# ChatGPT-o1-preview: python pacman?

import pygame
import sys

# Initialize pygame
pygame.init()

# Constants
SCREEN_WIDTH = 600
SCREEN_HEIGHT = 400
FPS = 60
PACMAN_SPEED = 5
GHOST_SPEED = 3

# Colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
YELLOW = (255, 255, 0)
RED = (255, 0, 0)

# Screen setup
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Pacman Game")

# Clock
clock = pygame.time.Clock()

# Pacman setup
pacman = pygame.Rect(300, 200, 20, 20)

# Ghost setup
ghost = pygame.Rect(100, 100, 20, 20)

# Score
score = 0

# Main game loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Movement keys
    keys = pygame.key.get_pressed()
    if keys[pygame.K_UP]:
        pacman.y -= PACMAN_SPEED
    if keys[pygame.K_DOWN]:
        pacman.y += PACMAN_SPEED
    if keys[pygame.K_LEFT]:
        pacman.x -= PACMAN_SPEED
    if keys[pygame.K_RIGHT]:
        pacman.x += PACMAN_SPEED

    # Ghost AI (simple movement)
    if ghost.x < pacman.x:
        ghost.x += GHOST_SPEED
    elif ghost.x > pacman.x:
        ghost.x -= GHOST_SPEED

    if ghost.y < pacman.y:
        ghost.y += GHOST_SPEED
    elif ghost.y > pacman.y:
        ghost.y -= GHOST_SPEED

    # Collision detection
    if pacman.colliderect(ghost):
        print("Game Over! Your score:", score)
        running = False

    # Draw everything
    screen.fill(BLACK)
    pygame.draw.rect(screen, YELLOW, pacman)  # Pacman
    pygame.draw.rect(screen, RED, ghost)     # Ghost

    # Display score
    font = pygame.font.SysFont(None, 36)
    score_text = font.render(f"Score: {score}", True, WHITE)
    screen.blit(score_text, (10, 10))

    pygame.display.flip()
    clock.tick(FPS)

pygame.quit()
sys.exit()

