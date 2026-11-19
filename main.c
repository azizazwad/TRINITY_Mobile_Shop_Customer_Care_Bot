#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Mobile Customer Care Bot
 * Console-based C project with persistent binary-file storage.
 * Build with: gcc -std=c99 -Wall -Wextra main.c -o main.exe
 */

#define PRODUCT_FILE "products.dat"
#define CUSTOMER_FILE "customers.dat"
#define ORDER_FILE "orders.dat"
#define WARRANTY_FILE "warranties.dat"
#define MAX_PRODUCTS 100
#define MAX_CUSTOMERS 1000
#define MAX_ORDERS 5000
#define MAX_WARRANTIES 5000

#define COLOR_RESET "\x1b[0m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_MAGENTA "\x1b[35m"

typedef struct {
    int id;
    char model[50];
    char details[120];
    double price;
} Product;

typedef struct {
    char name[60];
    char phone[24];
} Customer;

typedef struct {
    int id;
    char customerPhone[24];
    int productId;
    int quantity;
    double unitPrice;
    double deliveryFee;
    double total;
    int deliverySelected;
    char address[140];
    char imei[32];
    char warrantyId[24];
    time_t placedAt;
} Order;

typedef struct {
    char id[24];
    char imei[32];
    char customerPhone[24];
    int productId;
    time_t purchasedAt;
    int durationDays;
} Warranty;

static void read_line(const char *prompt, char *buffer, size_t size) {
    size_t length;
    int ch;
    if (prompt != NULL) {
        printf(COLOR_YELLOW "%s" COLOR_RESET, prompt);
    }
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else {
        while ((ch = getchar()) != '\n' && ch != EOF) { }
    }
}

static int read_int(const char *prompt, int minimum, int maximum) {
    char line[80];
    char *end;
    long value;
    for (;;) {
        read_line(prompt, line, sizeof(line));
        value = strtol(line, &end, 10);
        while (*end != '\0' && isspace((unsigned char)*end)) end++;
        if (line[0] != '\0' && *end == '\0' && value >= minimum && value <= maximum) {
            return (int)value;
        }
        printf("Please enter a number from %d to %d.\n", minimum, maximum);
    }
}

static int contains_ignore_case(const char *text, const char *query) {
    size_t i, j;
    if (query[0] == '\0') return 1;
    for (i = 0; text[i] != '\0'; i++) {
        for (j = 0; query[j] != '\0' && text[i + j] != '\0'; j++) {
            if (tolower((unsigned char)text[i + j]) != tolower((unsigned char)query[j])) break;
        }
        if (query[j] == '\0') return 1;
    }
    return 0;
}

static int append_apple_catalog(Product products[], int count) {
    static const Product appleProducts[] = {
        {1, "Apple iPhone Duo", "Apple US starting price USD 1999; BDT estimate at USD 1 = BDT 124.75; US eSIM-only model", 249375.0},
        {2, "Apple iPhone 18 Pro", "Apple US starting price USD 1199; BDT estimate at USD 1 = BDT 124.75; US eSIM-only model", 149575.0},
        {3, "Apple iPhone 18 Pro Max", "Apple US starting price USD 1299; BDT estimate at USD 1 = BDT 124.75; US eSIM-only model", 162050.0},
        {4, "Apple iPhone Air", "Apple US starting price USD 1099; BDT estimate at USD 1 = BDT 124.75; US eSIM-only model", 137100.0},
        {5, "Apple iPhone 17", "Apple US starting price USD 899; BDT estimate at USD 1 = BDT 124.75; US eSIM-only model", 112150.0},
        {6, "Apple iPhone 17e", "Apple US starting price USD 699; BDT estimate at USD 1 = BDT 124.75; US eSIM-only model", 87200.0},
        {7, "Apple iPhone 16", "Apple US starting price USD 799; BDT estimate at USD 1 = BDT 124.75; US eSIM-only model", 99675.0}
    };
    size_t i;
    for (i = 0; i < sizeof(appleProducts) / sizeof(appleProducts[0]) &&
                count < MAX_PRODUCTS; i++) {
        products[count++] = appleProducts[i];
    }
    return count;
}

static int load_apple_catalog(Product products[]) {
    return append_apple_catalog(products, 0);
}

static int matches_apple_catalog(const Product products[], int count) {
    Product dataset[MAX_PRODUCTS];
    int datasetCount = load_apple_catalog(dataset);
    int i;
    if (count != datasetCount) return 0;
    for (i = 0; i < count; i++) {
        if (products[i].id != dataset[i].id ||
            strcmp(products[i].model, dataset[i].model) != 0 ||
            strcmp(products[i].details, dataset[i].details) != 0 ||
            products[i].price != dataset[i].price) {
            return 0;
        }
    }
    return 1;
}

static int load_products(Product products[]) {
    FILE *file = fopen(PRODUCT_FILE, "rb");
    size_t count = 0;
    if (file != NULL) {
        count = fread(products, sizeof(Product), MAX_PRODUCTS, file);
        fclose(file);
    }
    {
        Product appleProducts[MAX_PRODUCTS];
        int appleCount = load_apple_catalog(appleProducts);
        if (appleCount <= 0) return (int)count;
        if (count != (size_t)appleCount || !matches_apple_catalog(products, (int)count)) {
            memcpy(products, appleProducts, (size_t)appleCount * sizeof(Product));
            file = fopen(PRODUCT_FILE, "wb");
            if (file != NULL) {
                fwrite(products, sizeof(Product), (size_t)appleCount, file);
                fclose(file);
            }
        } else {
            memcpy(products, appleProducts, (size_t)appleCount * sizeof(Product));
        }
        return appleCount;
    }
}

static int save_products(const Product products[], int count) {
    FILE *file = fopen(PRODUCT_FILE, "wb");
    int ok;
    if (file == NULL) return 0;
    ok = fwrite(products, sizeof(Product), (size_t)count, file) == (size_t)count;
    fclose(file);
    return ok;
}

static int load_customers(Customer customers[]) {
    FILE *file = fopen(CUSTOMER_FILE, "rb");
    size_t count = 0;
    if (file != NULL) {
        count = fread(customers, sizeof(Customer), MAX_CUSTOMERS, file);
        fclose(file);
    }
    return (int)count;
}

static int load_orders(Order orders[]) {
    FILE *file = fopen(ORDER_FILE, "rb");
    size_t count = 0;
    if (file != NULL) {
        count = fread(orders, sizeof(Order), MAX_ORDERS, file);
        fclose(file);
    }
    return (int)count;
}

static int load_warranties(Warranty warranties[]) {
    FILE *file = fopen(WARRANTY_FILE, "rb");
    size_t count = 0;
    if (file != NULL) {
        count = fread(warranties, sizeof(Warranty), MAX_WARRANTIES, file);
        fclose(file);
    }
    return (int)count;
}

static int find_product(const Product products[], int count, int id) {
    int i;
    for (i = 0; i < count; i++) if (products[i].id == id) return i;
    return -1;
}

static void print_product(const Product *product) {
    printf(COLOR_CYAN "[%d]" COLOR_RESET " %s | %s | Price: " COLOR_GREEN "Tk %.2f" COLOR_RESET "\n",
           product->id, product->model, product->details, product->price);
}

static void list_products(const Product products[], int count) {
    int i;
    printf("\n" COLOR_CYAN "--- Trinity Mobile Product Catalog ---" COLOR_RESET "\n");
    for (i = 0; i < count; i++) print_product(&products[i]);
    if (count == 0) printf("No products are currently listed.\n");
}

static int save_customer(const char *name, const char *phone) {
    Customer customers[MAX_CUSTOMERS];
    int count = load_customers(customers), i;
    FILE *file;
    for (i = 0; i < count; i++) {
        if (strcmp(customers[i].phone, phone) == 0) {
            snprintf(customers[i].name, sizeof(customers[i].name), "%s", name);
            file = fopen(CUSTOMER_FILE, "wb");
            if (file == NULL) return 0;
            i = (int)fwrite(customers, sizeof(Customer), (size_t)count, file);
            fclose(file);
            return i == count;
        }
    }
    if (count >= MAX_CUSTOMERS) return 0;
    snprintf(customers[count].name, sizeof(customers[count].name), "%s", name);
    snprintf(customers[count].phone, sizeof(customers[count].phone), "%s", phone);
    file = fopen(CUSTOMER_FILE, "ab");
    if (file == NULL) return 0;
    i = fwrite(&customers[count], sizeof(Customer), 1, file) == 1;
    fclose(file);
    return i;
}

static void register_customer(void) {
    char name[60], phone[24];
    read_line("Customer name: ", name, sizeof(name));
    read_line("Phone number: ", phone, sizeof(phone));
    if (name[0] == '\0' || phone[0] == '\0') {
        printf("Name and phone number are required.\n");
    } else if (save_customer(name, phone)) {
        printf("Customer record saved.\n");
    } else {
        printf("Could not save the customer record.\n");
    }
}

static void search_products(const Product products[], int count) {
    char query[80];
    int i, found = 0;
    read_line("Enter a model name or number: ", query, sizeof(query));
    for (i = 0; i < count; i++) {
        char id[16];
        snprintf(id, sizeof(id), "%d", products[i].id);
        if (contains_ignore_case(products[i].model, query) || contains_ignore_case(id, query)) {
            print_product(&products[i]);
            found = 1;
        }
    }
    if (!found) printf("No matching phone was found.\n");
}

static void compare_products(const Product products[], int count) {
    int first, second, a, b;
    if (count < 2) {
        printf("At least two products are needed for comparison.\n");
        return;
    }
    list_products(products, count);
    first = read_int("First product ID: ", 1, 999999);
    second = read_int("Second product ID: ", 1, 999999);
    a = find_product(products, count, first);
    b = find_product(products, count, second);
    if (a < 0 || b < 0 || a == b) {
        printf("Choose two different product IDs from the catalog.\n");
        return;
    }
    printf("\nComparison:\n");
    print_product(&products[a]);
    print_product(&products[b]);
}

static int save_order(const Order *order) {
    FILE *file = fopen(ORDER_FILE, "ab");
    int ok;
    if (file == NULL) return 0;
    ok = fwrite(order, sizeof(Order), 1, file) == 1;
    fclose(file);
    return ok;
}

static int save_warranty(const Warranty *warranty) {
    FILE *file = fopen(WARRANTY_FILE, "ab");
    int ok;
    if (file == NULL) return 0;
    ok = fwrite(warranty, sizeof(Warranty), 1, file) == 1;
    fclose(file);
    return ok;
}

static void show_order_receipt(const Order *order, const Product products[], int productCount) {
    int index = find_product(products, productCount, order->productId);
    char date[32] = "Unknown";
    struct tm *tmInfo = localtime(&order->placedAt);
    if (tmInfo != NULL) strftime(date, sizeof(date), "%Y-%m-%d %H:%M", tmInfo);
    printf("\n--- Order Receipt ---\nOrder ID: ORD-%06d\nDate: %s\n", order->id, date);
    printf("Customer phone: %s\n", order->customerPhone);
    if (index >= 0) printf("Phone: %s\n", products[index].model);
    printf("Quantity: %d\nUnit price: Tk %.2f\n", order->quantity, order->unitPrice);
    printf("Delivery: %s (Tk %.2f)\n", order->deliverySelected ? "Yes" : "Pickup", order->deliveryFee);
    if (order->deliverySelected) printf("Address: %s\n", order->address);
    printf("Total: Tk %.2f\n", order->total);
    printf("Warranty ID: %s\n", order->warrantyId);
}

static void place_order(const Product products[], int productCount) {
    static Order orders[MAX_ORDERS];
    Warranty warranty;
    Order order;
    int count, maxId = 0, i, productId, productIndex;
    int quantity, delivery;
    char name[60], phone[24], imei[32];
    if (productCount == 0) {
        printf("No products are available to order.\n");
        return;
    }
    list_products(products, productCount);
    productId = read_int("Choose product ID: ", 1, 999999);
    productIndex = find_product(products, productCount, productId);
    if (productIndex < 0) {
        printf("That product ID is not in the catalog.\n");
        return;
    }
    quantity = read_int("Quantity (1-5): ", 1, 5);
    read_line("Customer name: ", name, sizeof(name));
    read_line("Phone number: ", phone, sizeof(phone));
    if (name[0] == '\0' || phone[0] == '\0') {
        printf("Name and phone number are required to place an order.\n");
        return;
    }
    delivery = read_int("Delivery? 1 = deliver, 0 = pickup: ", 0, 1);
    memset(&order, 0, sizeof(order));
    memset(&warranty, 0, sizeof(warranty));
    if (delivery) read_line("Delivery address: ", order.address, sizeof(order.address));
    read_line("Phone IMEI/serial number for warranty: ", imei, sizeof(imei));
    if (imei[0] == '\0') {
        printf("An IMEI or serial number is required for warranty registration.\n");
        return;
    }
    if (!save_customer(name, phone)) {
        printf("Could not save customer information; order was not placed.\n");
        return;
    }
    count = load_orders(orders);
    for (i = 0; i < count; i++) if (orders[i].id > maxId) maxId = orders[i].id;
    order.id = maxId + 1;
    snprintf(order.customerPhone, sizeof(order.customerPhone), "%s", phone);
    order.productId = productId;
    order.quantity = quantity;
    order.unitPrice = products[productIndex].price;
    order.deliverySelected = delivery;
    order.deliveryFee = delivery ? 100.0 : 0.0;
    order.total = order.unitPrice * quantity + order.deliveryFee;
    snprintf(order.imei, sizeof(order.imei), "%s", imei);
    order.placedAt = time(NULL);
    snprintf(order.warrantyId, sizeof(order.warrantyId), "WAR-%06d", order.id);
    warranty.purchasedAt = order.placedAt;
    warranty.durationDays = 365;
    warranty.productId = productId;
    snprintf(warranty.id, sizeof(warranty.id), "%s", order.warrantyId);
    snprintf(warranty.imei, sizeof(warranty.imei), "%s", imei);
    snprintf(warranty.customerPhone, sizeof(warranty.customerPhone), "%s", phone);
    if (!save_order(&order)) {
        printf("Could not save the order.\n");
        return;
    }
    if (!save_warranty(&warranty)) {
        printf("Order saved, but warranty registration could not be saved. Contact staff with order ID ORD-%06d.\n", order.id);
        return;
    }
    show_order_receipt(&order, products, productCount);
    printf("Warranty registered for 12 months from purchase.\n");
}

static void find_order(const Product products[], int productCount) {
    static Order orders[MAX_ORDERS];
    char text[40];
    int count = load_orders(orders), i, id = -1;
    read_line("Enter order ID (for example ORD-000001): ", text, sizeof(text));
    if (sscanf(text, "ORD-%d", &id) != 1) id = atoi(text);
    for (i = 0; i < count; i++) {
        if (orders[i].id == id) {
            show_order_receipt(&orders[i], products, productCount);
            return;
        }
    }
    printf("No order was found with that ID.\n");
}

static void find_warranty(void) {
    static Warranty warranties[MAX_WARRANTIES];
    char query[40];
    int count = load_warranties(warranties), i;
    time_t now = time(NULL);
    read_line("Enter warranty ID or IMEI/serial number: ", query, sizeof(query));
    for (i = 0; i < count; i++) {
        if (strcmp(warranties[i].id, query) == 0 || strcmp(warranties[i].imei, query) == 0) {
            char purchase[32] = "Unknown", expiry[32] = "Unknown";
            time_t end = warranties[i].purchasedAt + (time_t)warranties[i].durationDays * 24 * 60 * 60;
            struct tm *purchaseTm = localtime(&warranties[i].purchasedAt);
            if (purchaseTm != NULL) strftime(purchase, sizeof(purchase), "%Y-%m-%d", purchaseTm);
            {
                struct tm *expiryTm = localtime(&end);
                if (expiryTm != NULL) strftime(expiry, sizeof(expiry), "%Y-%m-%d", expiryTm);
            }
            printf("Warranty ID: %s\nIMEI/serial: %s\nPurchase date: %s\n", warranties[i].id, warranties[i].imei, purchase);
            printf("Coverage: 12 months, until %s (%s)\n", expiry, now <= end ? "active" : "expired");
            return;
        }
    }
    printf("No warranty record was found.\n");
}

static void faq_chat(void) {
    char question[180];
    printf("\n" COLOR_MAGENTA "Trinity Care Bot:" COLOR_RESET " Tell me what you need help with. Type 'back' to return.\n");
    for (;;) {
        read_line("You: ", question, sizeof(question));
        if (contains_ignore_case(question, "back") || contains_ignore_case(question, "exit")) break;
        if (contains_ignore_case(question, "price") || contains_ignore_case(question, "cost")) {
            printf("Bot: Product prices are listed in the catalog and stay fixed in this demo.\n");
        } else if (contains_ignore_case(question, "delivery") || contains_ignore_case(question, "shipping")) {
            printf("Bot: Choose delivery during checkout. Demo delivery costs Tk 100; pickup has no delivery fee.\n");
        } else if (contains_ignore_case(question, "warranty") || contains_ignore_case(question, "guarantee")) {
            printf("Bot: A 12-month warranty is registered during purchase. Keep your warranty ID and IMEI/serial number.\n");
        } else if (contains_ignore_case(question, "battery") || contains_ignore_case(question, "charge")) {
            printf("Bot: Try a known-good charger and cable, inspect the charging port, and restart the phone. Stop if it becomes hot or swollen and contact service.\n");
        } else if (contains_ignore_case(question, "internet") || contains_ignore_case(question, "wifi") || contains_ignore_case(question, "network")) {
            printf("Bot: Toggle airplane mode, restart the phone, and check that Wi-Fi or mobile data is enabled. If needed, contact your carrier.\n");
        } else if (contains_ignore_case(question, "screen") || contains_ignore_case(question, "display")) {
            printf("Bot: Restart the phone and check for visible damage. If the display remains unresponsive, contact an authorized service center.\n");
        } else if (contains_ignore_case(question, "order") || contains_ignore_case(question, "purchase")) {
            printf("Bot: Use Place an Order to make a demo purchase, or Order Lookup to retrieve a saved receipt.\n");
        } else {
            printf("Bot: I can help with catalog prices, delivery, warranty, orders, battery, network, and screen issues. Try one of those topics.\n");
        }
    }
}

static void add_or_update_product(Product products[], int *count) {
    int id, index;
    Product product;
    if (*count >= MAX_PRODUCTS) {
        printf("The product catalog is full.\n");
        return;
    }
    id = read_int("Product ID (enter existing ID to update): ", 1, 999999);
    index = find_product(products, *count, id);
    memset(&product, 0, sizeof(product));
    product.id = id;
    read_line("Model name: ", product.model, sizeof(product.model));
    read_line("Specifications: ", product.details, sizeof(product.details));
    product.price = (double)read_int("Fixed price in Tk (whole number): ", 1, 100000000);
    if (product.model[0] == '\0') {
        printf("Model name cannot be empty.\n");
        return;
    }
    if (index >= 0) products[index] = product;
    else products[(*count)++] = product;
    if (save_products(products, *count)) printf("Catalog record saved.\n");
    else printf("Could not save the product catalog.\n");
}

static void admin_records(void) {
    static Order orders[MAX_ORDERS];
    static Warranty warranties[MAX_WARRANTIES];
    int orderCount = load_orders(orders), warrantyCount = load_warranties(warranties), i;
    printf("\n--- Saved Orders (%d) ---\n", orderCount);
    for (i = 0; i < orderCount; i++) {
        printf("ORD-%06d | %s | Product %d | Qty %d | Tk %.2f\n", orders[i].id,
               orders[i].customerPhone, orders[i].productId, orders[i].quantity, orders[i].total);
    }
    printf("\n--- Warranty Records (%d) ---\n", warrantyCount);
    for (i = 0; i < warrantyCount; i++) {
        printf("%s | %s | %s\n", warranties[i].id, warranties[i].customerPhone, warranties[i].imei);
    }
}

static void staff_menu(Product products[], int *productCount) {
    char pin[20];
    int choice;
    read_line("Staff PIN (demo PIN: 2468): ", pin, sizeof(pin));
    if (strcmp(pin, "2468") != 0) {
        printf("Incorrect PIN.\n");
        return;
    }
    do {
        printf("\n" COLOR_CYAN "--- Staff Menu ---" COLOR_RESET "\n1. Add or update a product\n2. View saved orders and warranties\n0. Return\n");
        choice = read_int("Choose: ", 0, 2);
        if (choice == 1) add_or_update_product(products, productCount);
        else if (choice == 2) admin_records();
    } while (choice != 0);
}

int main(void) {
    Product products[MAX_PRODUCTS];
    int productCount = load_products(products);
    int choice;
    printf(COLOR_CYAN "=====================================\n");
    printf("      TRINITY MOBILE CARE BOT\n");
    printf("  Hello! How can I help you today?\n");
    printf("=====================================\n" COLOR_RESET);
    do {
        printf("\n" COLOR_GREEN "1. Browse products and prices\n2. Search for a phone\n3. Compare phones\n");
        printf("4. Register customer\n5. Place an order\n6. Look up an order\n");
        printf("7. Look up a warranty\n8. Chat with customer care bot\n9. Staff menu\n0. Exit\n" COLOR_RESET);
        choice = read_int("Choose an option: ", 0, 9);
        switch (choice) {
            case 1: list_products(products, productCount); break;
            case 2: search_products(products, productCount); break;
            case 3: compare_products(products, productCount); break;
            case 4: register_customer(); break;
            case 5: place_order(products, productCount); break;
            case 6: find_order(products, productCount); break;
            case 7: find_warranty(); break;
            case 8: faq_chat(); break;
            case 9: staff_menu(products, &productCount); break;
            case 0: printf("Thank you for contacting Trinity Mobile Care. Goodbye!\n"); break;
        }
    } while (choice != 0);
    return 0;
}
