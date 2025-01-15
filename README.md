# ZeroProject - Third-Person Stealth Horror Code Showcase

Welcome to **ZeroProject** source code repository, a stealth horror game developed in Unreal Engine using C++. This repository serves as a **code showcase** and is not intended as a full game distribution. The purpose of this repo is to showcase the coded gameplay systems, and design patterns used in this Unreal Engine C++ project. 

The **game demo** will soon be available on my **[Itch.io](https://loreark.itch.io/)** page.

## Project Overview
- **Type**: Third-Person Stealth Horror
- **Engine**: Unreal Engine
- **Language**: Primarily developed in C++, Blueprints implementations

## Main Features
- **Characters and Pawn Possession System**:
  - **Character Swapping**: Seamlessly switch between player-controlled characters and enemy characters.
  - **Player Character (Third-Person)**: Possess or Sightjack enemies, navigate the environment, gather items, craft magic seals, and engage in stealth and combat actions.
  - **Enemy Character (First-Person)**: Controlled during possession for combat or strategic movement, with unique weapon-based movesets for both player possession and AI.
  - **Player Dummy Character (Third-Person)**: A limited-action copy of the player character, used for safe exploration and enemy distraction.
  - **Player Ally Character (AI-Controlled)**: Summoned by the player to execute tasks (e.g., Move To, Follow, Attack).
    
- **Magic Seals Crafting System**
  - Craft and use magic seals with various gameplay effects, supporting stealth or combat approaches.

- **AI System**:
  - **State Machine and Behavior Tree AI Controller Logic**:
    - States: Passive, Investigating, Combat, Seeking.

  - **Task-based patrol system**: AI patrols with time based task assignable at each waypoint.

- **Enemy AI Perception System**:
  - Custom AI perception component to adjust awareness based on the current state.
  - **Team-based Attack Target Selection**: Dynamically adapts to situations based on priority logic, allowing enemies to select targets based on the current scenario. Possible targets include:
      - Hostile (player-possessed) enemies
      - Player
      - Player Dummy
      - Player Ally 

- **Level Management**
  - Level loading system.
  - Checkpoint system.

## Code Structure
- **Source**: The repository contains the core C++ source code used in the game.
- **Exclusions**: All paid assets, game content, and non-code resources (e.g., models, textures, animations) are excluded to comply with licensing requirements and because they are outside the scope of this repository.

## Disclaimer
- This repository is not a full Unreal Engine project but contains only the source code for showcasing the gameplay systems and design patterns implemented. It is the source code of a work-in-progress project.

## Contributors
 - Lorenzo Pusateri (Solo Developer).

## Contact
For questions or feedback, feel free to contact me via my [portfolio website](https://lorenzopusateri.wordpress.com//).
