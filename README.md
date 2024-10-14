# Open source version of [Games bot](https://gamesbot.lesikr.com/).


# NOTE:
## General
This repository does contain only source code. 
You would not be able to compile it and use as it does not contain database, config template, images, dependencies, etc. 
This code can be only used for reference proposes. 
I do not provide any support regarding it.

## Work in progress.
this repository is not direct open sourcing of games bot, it is full remake of it using coroutines
and full redesign of architecture.  

## TO DO
- [X] 2048
- [X] achievements
- [X] battleships
- [X] chess
- [X] connect_four
- [X] dominoes
- [X] global_stats
- [X] hangman
- [X] help
- [X] local_stats
- [X] minesweeper
- [X] ping
- [X] premium
- [X] puzzle_15
- [X] rubiks_cube
- [X] sudoku
- [X] tic_tac_toe
- [ ] website
- [X] reports to websites like top.gg
- [X] statistics collection


# Implementation details

## Modules
Every module is a runtime linking .so which inherits [Module](https://github.com/ruslan-ilesik/games_bot/blob/main/src/module/module.hpp) class

### Basic principal 
Every module inherits basic module and implements 3 methods `init`, `run`, `stop` and also `create` factory function which will create module
and return shared_ptr to it.

### In practice (Abstract classes + Factories)
Every module (if it is planned to be accessible) should have an abstract class of it which will be included into
Modules using it defining all public methods and variables, And all the implementation should be separated
into different file, this way we can decrease headers parsing and remove need to link dependencies of current
module to another when it is not needed.

### Dependencies
Every module has its own name and dependencies list (list of names of modules this module is dependent on).
[Modules_manager](https://github.com/ruslan-ilesik/games_bot/blob/main/src/module/modules_manager.hpp) will handle them and load in appropriate order,
also it is made to notify about missing or circular dependencies.

## Admin terminal
Bot implements admin terminal running with executable. Each command can add its own command to terminal at runtime which 
can be run by owner. For example, manually load/stop modules, forbidding modules loading, adding new connection
to Database pool, etc.

## Database
[Database](https://github.com/ruslan-ilesik/games_bot/blob/main/src/modules/database/) is based on using combination of **connections pool** and **C++20 coroutines**, you can define amount of
connections which should be open and executed in parallel and use coroutines approach to create sql queries. 
**No more callback hell!!!**

### Basic principal (Coroutines + connection pool)

When you create task of sql request it is automatically adds itself to the queue and could be 
picked up by one of open connections. So by the time you call co_await (somewhere later in the code) you can already 
have response from database which will make awaiting instant as it will just return value. 

### Prepared statements
Database module also supports prepared statements and executing them is made as template so no need to predefine parameters amount or their types.

### Background queries
Support for background queries, if you need to insert something why make user wait for it to happen, just do request in background.

## Components and interactions
All the components ids are hidden from user to prevent self-bots make automation based on ids.

### Details
We have created [Id_cache](https://github.com/ruslan-ilesik/games_bot/tree/main/src/modules/discord/discord_interactions_handler)
class for automating id hiding. it uses 64-bit unsigned integer to map them discord components ids.
to store ids at runtime which are always cleared when no needed anymore. They are randomly chosen every time, so 
same custom_id will not produce same hidden id which is passed to discord and user.

### Usage
This approach is used in [select_click_handler](https://github.com/ruslan-ilesik/games_bot/tree/main/src/modules/discord/discord_interactions_handler/discord_select_menu_handler)
and [button_click_handler](https://github.com/ruslan-ilesik/games_bot/tree/main/src/modules/discord/discord_interactions_handler/discord_button_click_handler) 
to make life of self-bots a bit harder (self-bots are forbidden by TOS of discord).

Interaction handlers implement way to await interactions on specific message using our unique ids, check for specific users allow to interact and timeouts which make them really handy.

### Additional
As advantage now the length of our custom ids basically has no limit (discord limits custom component ids to 100 bytes) in size so we can store as much data as we want there.
