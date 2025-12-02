/*
Sistema de Loja Online - Versão API REST (C++ com cpp-httplib)

Este documento contém:
- Modelagem UML (PlantUML)
- Código fonte: implementação das classes do domínio (Product, Category, Customer, Order, Store)
  e um servidor REST simples usando cpp-httplib (single-header) e nlohmann::json (single-header).
- Endpoints REST básicos para gerenciar produtos, carrinho e checkout.

Dependências (single-header recomendadas):
- cpp-httplib: https://github.com/yhirose/cpp-httplib (arquivo httplib.h)
- nlohmann/json: https://github.com/nlohmann/json (arquivo json.hpp)

Compilação (exemplo):
- g++ -std=c++17 -O2 -pthread -o loja_server SistemaLojaOnlineServer.cpp
  (coloque httplib.h e json.hpp no mesmo diretório ou em include path)

Observações: esta é uma API em memória, sem persistência. Para produção adicione banco (SQLite/Postgres), segurança e validação.

---------- Modelagem UML (PlantUML) ----------

@startuml
class Product {
  -int id
  -string name
  -string description
  -double price
  -int stock
  +getId(): int
}
class Customer {
  -int id
  -string name
  -string email
  -vector<CartItem> cart
}
class Order {
  -int id
  -vector<CartItem> items
  -double total
}
class Store {
  -vector<Product> products
  -vector<Category> categories
  +findProductById(id: int): Product*
  +placeOrder(o: Order): bool
}
Customer "1" *-- "*" CartItem
Order "1" *-- "*" CartItem
Store "1" o-- "*" Product
@enduml

---------- Código: SistemaLojaOnlineServer.cpp ----------

// O código abaixo implementa o domínio e um servidor REST simples.
// Ele usa httplib (cpp-httplib) e nlohmann::json.

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <mutex>

#include "httplib.h"        // coloque httplib.h no include path
#include "json.hpp"        // coloque json.hpp (nlohmann) no include path

using namespace std;
using json = nlohmann::json;

// ---------- Entidades (sem alterações conceituais) ----------
class Product {
private:
    int id;
    string name;
    string description;
    double price;
    int stock;
public:
    Product() = default;
    Product(int id, string name, string desc, double price, int stock)
        : id(id), name(move(name)), description(move(desc)), price(price), stock(stock) {}

    int getId() const { return id; }
    const string& getName() const { return name; }
    const string& getDescription() const { return description; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    bool decreaseStock(int qty) {
        if (qty <= 0) return false;
        if (qty > stock) return false;
        stock -= qty;
        return true;
    }
    void increaseStock(int qty) { if (qty>0) stock += qty; }

    json toJson() const {
        return json{{"id", id},{"name", name},{"description", description},{"price", price},{"stock", stock}};
    }

    static Product fromJson(const json &j) {
        return Product(j.value("id", 0), j.value("name", string()), j.value("description", string()), j.value("price", 0.0), j.value("stock", 0));
    }
};

struct CartItem {
    int productId;
    string productName;
    double unitPrice;
    int qty;
    double subtotal() const { return unitPrice * qty; }
    json toJson() const { return json{{"productId", productId},{"productName", productName},{"unitPrice", unitPrice},{"qty", qty},{"subtotal", subtotal()}}; }
};

class Order {
private:
    int id;
    vector<CartItem> items;
    double total;
public:
    Order(int id=0, vector<CartItem> items = {}): id(id), items(move(items)), total(0.0) { calculateTotal(); }
    void calculateTotal() {
        total = 0.0;
        for (const auto &it : items) total += it.subtotal();
    }
    double getTotal() const { return total; }
    int getId() const { return id; }
    const vector<CartItem>& getItems() const { return items; }
    json toJson() const {
        json arr = json::array();
        for (const auto &it : items) arr.push_back(it.toJson());
        return json{{"id", id},{"items", arr},{"total", total}};
    }
};

// ---------- Repositório/Loja em memória ----------
class Store {
private:
    vector<Product> products;
    int nextOrderId = 1;
    mutex mtx; // proteção concorrência
public:
    Store() = default;

    void addProduct(const Product &p) { lock_guard<mutex> lock(mtx); products.push_back(p); }
    Product* findProductById(int id) { lock_guard<mutex> lock(mtx); for (auto &p : products) if (p.getId()==id) return &p; return nullptr; }
    vector<Product> listProducts() { lock_guard<mutex> lock(mtx); return products; }

    int generateOrderId() { lock_guard<mutex> lock(mtx); return nextOrderId++; }

    bool placeOrder(const Order &o, string &err) {
        lock_guard<mutex> lock(mtx);
        // verificar estoque
        for (const auto &it : o.getItems()) {
            Product *p = nullptr;
            for (auto &pp : products) if (pp.getId()==it.productId) { p = &pp; break; }
            if (!p) { err = "Produto não encontrado: " + to_string(it.productId); return false; }
            if (p->getStock() < it.qty) { err = "Estoque insuficiente para: " + p->getName(); return false; }
        }
        // reduzir estoque
        for (const auto &it : o.getItems()) {
            for (auto &pp : products) if (pp.getId()==it.productId) { pp.decreaseStock(it.qty); break; }
        }
        return true;
    }
};

// ---------- Sessão simples por cliente (em memória) ----------
// A API vai manter "carrinhos" por clienteId (simples map). Em produção, use autenticação.
#include <unordered_map>
class SessionManager {
private:
    unordered_map<int, vector<CartItem>> carts; // clienteId -> cart items
    mutex mtx;
public:
    void addToCart(int customerId, const CartItem &item) {
        lock_guard<mutex> lock(mtx);
        auto &cart = carts[customerId];
        // mesclar se existir
        for (auto &ci : cart) if (ci.productId==item.productId) { ci.qty += item.qty; return; }
        cart.push_back(item);
    }
    vector<CartItem> getCart(int customerId) { lock_guard<mutex> lock(mtx); return carts[customerId]; }
    void clearCart(int customerId) { lock_guard<mutex> lock(mtx); carts.erase(customerId); }
};

// ---------- Servidor REST (endpoints básicos) ----------
int main() {
    Store store;
    SessionManager sessions;

    // Popular com alguns produtos de exemplo
    store.addProduct(Product(1, "Teclado Mecânico", "Teclado retroiluminado", 299.90, 10));
    store.addProduct(Product(2, "Mouse Gamer", "Mouse com alta precisão", 149.50, 5));
    store.addProduct(Product(3, "Monitor 24-inch", "Full HD 75Hz", 899.00, 2));

    httplib::Server svr;

    // GET /products -> lista todos
    svr.Get("/products", [&](const httplib::Request&, httplib::Response &res){
        auto prods = store.listProducts();
        json arr = json::array();
        for (const auto &p : prods) arr.push_back(p.toJson());
        res.set_content(arr.dump(4), "application/json");
    });

    // GET /product?id=1 -> obtém produto por id via query string
    svr.Get("/product", [&](const httplib::Request &req, httplib::Response &res){
        if (req.has_param("id")) {
            int id = stoi(req.get_param_value("id"));
            Product *p = store.findProductById(id);
            if (!p) { res.status = 404; res.set_content("{\"error\":\"Produto não encontrado\"}", "application/json"); return; }
            res.set_content(p->toJson().dump(), "application/json");
            return;
        }
        res.status = 400; res.set_content("{\"error\":\"Parâmetro id necessário\"}", "application/json");
    });

    // POST /cart/add  -> body JSON: {"customerId":1, "productId":2, "qty":1}
    svr.Post("/cart/add", [&](const httplib::Request &req, httplib::Response &res){
        try {
            auto j = json::parse(req.body);
            int customerId = j.value("customerId", 0);
            int productId = j.value("productId", 0);
            int qty = j.value("qty", 1);
            if (customerId<=0 || productId<=0 || qty<=0) { res.status=400; res.set_content("{\"error\":\"Parâmetros inválidos\"}", "application/json"); return; }
            Product *p = store.findProductById(productId);
            if (!p) { res.status=404; res.set_content("{\"error\":\"Produto não encontrado\"}", "application/json"); return; }
            if (p->getStock() <= 0) { res.status=400; res.set_content("{\"error\":\"Produto sem estoque\"}", "application/json"); return; }
            CartItem item{p->getId(), p->getName(), p->getPrice(), qty};
            sessions.addToCart(customerId, item);
            res.set_content("{\"ok\":true}", "application/json");
        } catch (...) { res.status=400; res.set_content("{\"error\":\"JSON inválido\"}", "application/json"); }
    });

    // GET /cart?customerId=1
    svr.Get("/cart", [&](const httplib::Request &req, httplib::Response &res){
        if (!req.has_param("customerId")) { res.status=400; res.set_content("{\"error\":\"Parâmetro customerId necessário\"}", "application/json"); return; }
        int customerId = stoi(req.get_param_value("customerId"));
        auto cart = sessions.getCart(customerId);
        json arr = json::array(); double sum=0;
        for (const auto &it : cart) { arr.push_back(it.toJson()); sum += it.subtotal(); }
        json out{{"customerId", customerId},{"items", arr},{"subtotal", sum}};
        res.set_content(out.dump(4), "application/json");
    });

    // POST /checkout -> body JSON: {"customerId":1}
    svr.Post("/checkout", [&](const httplib::Request &req, httplib::Response &res){
        try {
            auto j = json::parse(req.body);
            int customerId = j.value("customerId", 0);
            if (customerId<=0) { res.status=400; res.set_content("{\"error\":\"customerId inválido\"}", "application/json"); return; }
            auto cart = sessions.getCart(customerId);
            if (cart.empty()) { res.status=400; res.set_content("{\"error\":\"Carrinho vazio\"}", "application/json"); return; }
            int orderId = store.generateOrderId();
            Order order(orderId, cart);
            string err;
            if (!store.placeOrder(order, err)) { res.status=400; json out{{"ok", false},{"error", err}}; res.set_content(out.dump(), "application/json"); return; }
            sessions.clearCart(customerId);
            res.set_content(order.toJson().dump(4), "application/json");
        } catch (...) { res.status=400; res.set_content("{\"error\":\"JSON inválido\"}", "application/json"); }
    });

    cout << "Servidor rodando em http://localhost:8080
";
    svr.listen("0.0.0.0", 8080);
    return 0;
}


---------- Endpoints (resumo) ----------
- GET  /products             -> lista todos os produtos
- GET  /product?id={id}     -> obtém produto por id (query string)
- POST /cart/add             -> adiciona item ao carrinho (JSON: customerId, productId, qty)
- GET  /cart?customerId={id} -> visualiza carrinho
- POST /checkout             -> efetua checkout (JSON: customerId)

---------- Instruções rápidas de teste (curl) ----------
1) Listar produtos:
   curl http://localhost:8080/products

2) Adicionar ao carrinho (cliente 1 adiciona 2 unidades do produto 1):
   curl -X POST -H "Content-Type: application/json" -d '{"customerId":1,"productId":1,"qty":2}' http://localhost:8080/cart/add

3) Ver carrinho:
   curl http://localhost:8080/cart?customerId=1

4) Checkout:
   curl -X POST -H "Content-Type: application/json" -d '{"customerId":1}' http://localhost:8080/checkout

---------- Próximos passos sugeridos ----------
- Adicionar persistência com SQLite (ex.: sqlite3 + wrapper) para produtos, pedidos e sessões.
- Implementar autenticação (JWT) e gerenciamento de usuários (customers/admins).
- Validação e sanitização de entrada mais robusta.
- Adicionar logs, métricas e testes automatizados.

*/
