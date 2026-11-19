<div align="center">

# TRINITY | Mobile Customer Care Bot

**C console application · Customer care and phone catalog demo**

</div>

> [!NOTE]
> A local console demo with a continuously available menu and prepared-response
> chatbot. It does not connect to an online store, payment service, delivery
> company, or live agent.

## Project at a glance

| **Project** | **Team** | **Members** |
|:---|:---|:---|
| Mobile Customer Care Bot | TRINITY | Alif · Aziz · Rony |

## Build and run

| **Platform** | **Build** | **Run** |
|:---|:---|:---|
| Windows | `gcc -std=c99 -Wall -Wextra main.c -o main.exe` | `main.exe` |
| Linux / macOS | `gcc -std=c99 -Wall -Wextra main.c -o main` | `./main` |

The program uses standard C libraries only. Run it from the directory where you
want its data files stored.

## What you can do

| **Area** | **Available actions** |
|:---|:---|
| Phone catalog | Browse, search, and compare phones and prices |
| Orders | Place an order, choose delivery or pickup, and look up an order |
| Warranty | Register a 12-month warranty during purchase and look up a warranty |
| Customer care | Chat about catalog prices, delivery, warranty, orders, battery, network, and screen issues |
| Staff tools | Add or update a catalog item and view saved orders and warranties |

## Product catalog

The built-in catalog contains seven iPhone entries based on the US Apple Store.
Their US starting prices are converted to BDT using the Bangladesh Bank rate of
**USD 1 = BDT 124.75** (2026-11-19). These are estimates, not Bangladesh retail
prices, and exclude local taxes and import costs. The separate
`mobiledokan_phones.csv` reference file is not loaded into the app catalog.

> **Catalog behavior**  
> At startup, the app restores `products.dat` to the built-in Apple catalog.
> Staff catalog edits are reset the next time the program starts. Orders,
> customers, and warranties are stored separately and are unaffected.

## Local data files

| **File** | **Stores** |
|:---|:---|
| `products.dat` | Built-in phone catalog |
| `customers.dat` | Registered customer records |
| `orders.dat` | Order and delivery records |
| `warranties.dat` | Warranty registrations |

The program creates these files in its current working directory when needed.

## Order details

| **Item** | **Details** |
|:---|:---|
| Warranty | 12 months; registered at purchase with the entered IMEI or serial number |
| Delivery | Demo fee: Tk 100 |
| Pickup | No delivery fee |
| Staff demo PIN | `2468` |
