# 🎮 Open Source Version of [Games Bot](https://gamesbot.lesikr.com/)

## ⚠️ NOTE:
This repository contains only the **source code**. It is **not** a complete, ready-to-run bot as it lacks:
- Database setup
- Config templates
- Images
- Dependencies

It is **for reference purposes only** and **does not include support**.

## 🛠️ Implementation Details

### ⚙️ Modules
Each module is a **runtime-linked .so** file that inherits the [Module](https://github.com/ruslan-ilesik/games_bot/blob/main/src/module/module.hpp) class.

#### 📜 Basic Principle
Every module inherits the basic module class and implements three methods:
- `init()`
- `run()`
- `stop()`

Additionally, a `create` factory function is used to generate and return a `shared_ptr` to the module.

### 🔧 In Practice (Abstract Classes + Factories)
Modules are designed to be accessible through **abstract classes**, which define all public methods and variables. The **implementation** is separated into different files. This design improves **compilation speed** and reduces unnecessary linking between unrelated modules.

### 🔗 Dependencies
Every module comes with a **name** and a list of **dependencies** (other modules it depends on). The [Modules Manager](https://github.com/ruslan-ilesik/games_bot/blob/main/src/module/modules_manager.hpp) takes care of loading the modules in the correct order and ensures there are no **missing** or **circular** dependencies.

## 🖥️ Admin Terminal
The bot features an **admin terminal**, which runs as an executable. This allows the bot owner to:
- **Load/stop modules** manually
- **Restrict module loading**
- **Add new database connections**

The terminal provides a powerful and flexible way to manage your bot during runtime.

## 💾 Database
The bot uses a **connection pool** and **C++20 coroutines** for efficient database interaction. It supports:
- **Prepared statements** for flexibility and performance
- **Background queries** to avoid blocking user requests
- **And more**. U can define any commands

### 🚀 Basic Principle (Coroutines + Connection Pool)
SQL requests are queued up and automatically picked by one of the available database connections. When you call `co_await`, the task is already handled in the background, making awaiting **instant** and more efficient.

### 🔒 No More Callback Hell!
Coroutines simplify asynchronous programming, eliminating the complex nesting of callbacks. The database module allows you to execute queries without worrying about the callback hell.

## 🔑 Components and Interactions
The bot hides **component IDs** to prevent self-bots from automating actions. This is achieved using a **hidden ID** system with **randomized mappings** to avoid exposure of sensitive data like Discord component IDs.

### 🔐 Interaction Handlers
Interaction handlers manage actions like button clicks and select menus, making it difficult for **self-bots** to exploit the bot. The bot uses unique IDs for each interaction, which:
- Are randomized and hidden
- Prevent self-bots from automating actions
- Allow for **time-sensitive** interactions

### 🧩 Advantages
- **Unlimited custom ID length**: No more 100-byte limit from Discord.
- **Dynamic handling**: Efficiently handles button presses and select menu choices.

## 🌐 Webserver
The bot integrates a **webserver module**, which serves as a **REST API** for web-based interactions. This module supports:
- **Cookies and JWT Authentication** for secure session management
- **OAuth2 Integration** with **Discord** and **Patreon**

### 🔧 Key Features
- **REST API**: Exposes API endpoints for easy interaction between web pages and bot modules.
- **Cookies & JWT Authentication**: Secure session handling, ensuring user data is protected.
- **Asynchronous requests**: The webserver handles requests asynchronously using **C++20 coroutines**, ensuring fast and responsive performance even under heavy load.

### 💳 OAuth2 Integration for Discord and Patreon
The webserver allows users to log in via **Discord** or **Patreon**, granting access to personalized features based on their accounts.

### 📬 Handling Patreon Webhooks
The bot supports **Patreon webhooks** to automatically update user data in real time:
- **Webhook Listener**: Listens to webhook events such as pledges, updates, or cancellations.
- **Real-Time Updates**: Updates user membership data in the database as soon as a webhook is received.
- **Secure Verification**: Ensures webhook authenticity by verifying the signature headers.
- **Customizable Actions**: Specific actions are triggered based on webhook events, such as granting premium features.
