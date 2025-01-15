# ZeroProject - Third-Person Stealth Horror Code Showcase

Welcome to **ZeroProject** source code repository, a stealth horror game developed in Unreal Engine using C++. This repository serves as a **code showcase** and is not intended as a full game distribution. The purpose of this repo is to showcase the coded gameplay systems, and design patterns used in this Unreal Engine C++ project. 

The **game demo** will soon be available on my **[Itch.io](https://loreark.itch.io/)** page.

## Project Overview
- **Type**: Third-Person Stealth Horror
- **Engine**: Unreal Engine
- **Language**: Primarily developed in C++, Blueprints implementations

## Main Features
- **Character Pawn Possession System**:
  - Character Swapping: Seamlessly switch between player-controlled characters and enemy characters.
  - Player Character (Third-Person): Initiates possession of enemies and navigates the environment, handle item gathering, magic seals crafting, stealth and combat actions.
  - Enemy Character (First-Person): Controlled during possession to engage in combat or strategic movement. Variety based on weapons with unique movesets for both player possession and AI.

- **Magic Seals Crafting System**
  - Magic Seals with various gameplay effect, for stealth or combat approach.

- **AI System**:
  - State Machine and Behavior Tree AI Controller Logic.
    - States: Passive, Investigating, Combat, Seeking.

  - Task-based patrol system.
  - Dynamic Target Selection: Team-based system for responsive AI engagement..
    
- **AI Perception System**:
  - Custom perception system component based, to customize AI awerness based on the current state.
  - Team system for dynamic target selection.

- **Level Management**
  - Level loading system.
  - Checkpoint system.

## Code Structure
- **Source**: The repository contains the core C++ source code used in the game.
- **Exclusions**: All paid assets, game content, and non-code resources (like models, textures, animations, etc.) have been excluded to comply with licensing requirements.

## Disclaimer
- **Gameplay**: This is not a fully playable game. It is a work-in-progress project meant to show code and system logic.

## Contact
For questions or feedback, feel free to contact me via my [portfolio website](https://lorenzopusateri.wordpress.com//).
