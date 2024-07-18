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

# Implementation details

## Modules
Every module is a runtime linking .so which inherits [Module](https://github.com/ruslan-ilesik/games_bot/blob/main/src/module/module.hpp) class

### Basic principal 
Every module inherits basic module and implements 3 methods `innit`, `run` and `stop` and also `create` factory function which will create module
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


## Database
[Database](https://github.com/ruslan-ilesik/games_bot/blob/main/src/modules/database/) is based on using combination of **connections pool** and **C++20 coroutines**, you can define amount of
connections which should be open and executed in parallel and use coroutines approach to create sql queries. 
**No more callback hell!!!**

### Basic principal (Coroutines + connection pool)

When you create task of sql request it is automatically adds itself to the queue and could be 
picked up by one of open connections. So by the time you call co_await (somewhere later in the code) you can already 
have response from database which will make awaiting instant as it will just return value. 

### Background queries
Support for background queries, if you need to insert something why make user wait for it to happen, just do request in background.

