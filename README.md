# README – Sistema de Loja Online em C++

Este repositório reúne o desenvolvimento completo de um Sistema de Loja Online implementado em C++, aplicando Programação Orientada a Objetos (POO), Modelagem UML e integração com uma API REST utilizando cpp-httplib.
O projeto foi estruturado com foco educacional, demonstrando desde a modelagem conceitual até a implementação prática de um sistema modular e escalável.

## Sobre o Projeto

Este projeto consiste no desenvolvimento de um **Sistema de Loja Online** utilizando **C++**, aplicando conceitos de **Programação Orientada a Objetos (POO)**, **Modelagem UML** e posteriormente a implementação de uma **API REST** utilizando a biblioteca **cpp-httplib**. O objetivo é demonstrar a estruturação de um sistema real, modular e escalável.

---

## Estrutura do Sistema

O sistema foi modelado para simular operações básicas de um e-commerce.

### Classes Principais:

* **Produto** – representa itens da loja.
* **Carrinho** – gerencia produtos adicionados por um cliente.
* **Cliente** – representa o usuário comprador.
* **Loja** – gerencia o catálogo, estoque e operações gerais.
* **API (httplib)** – provê endpoints REST para interação externa.

---

## Conceitos de POO Aplicados

* **Encapsulamento** – atributos protegidos e acesso por getters/setters.
* **Abstração** – classes representam entidades reais do domínio.
* **Herança** – possibilidade de expansão futura (ex.: ProdutoDigital).
* **Polimorfismo** – métodos sobrescritos em subclasses (quando aplicável).

---

## Tecnologias Utilizadas

* **C++17**
* **Biblioteca httplib.h** (API REST)
* **nlohmann::json** (manipulação JSON)
* **PlantUML** (Modelagem UML)
* **G++ / MinGW / Clang**

---

## Como Executar o Projeto

```bash
g++ main.cpp -o loja -std=c++17
./loja
```

Ou, se utilizar API REST:

```bash
g++ api.cpp -o api -std=c++17
./api
```

---

## 📂 Estrutura do Projeto

```
📦 SistemaLojaOnline
├── src/
│   ├── produto.h
│   ├── cliente.h
│   ├── carrinho.h
│   ├── loja.h
│   ├── main.cpp
│   └── api.cpp
├── uml/
│   └── diagrama.puml
├── README.md
└── docs/
```

---

## Objetivo do Sistema

Criar uma solução que simule o comportamento de uma loja virtual, incluindo catálogo de produtos, carrinho de compras e checkout, demonstrando boas práticas de programação e arquitetura.

---

## Levantamento de Requisitos

### Requisitos Funcionais

* RF01 – Listar produtos do catálogo.
* RF02 – Consultar produto por ID.
* RF03 – Adicionar produto ao carrinho.
* RF04 – Visualizar itens do carrinho.
* RF05 – Finalizar compra (checkout).
* RF06 – Persistência ou simulação em memória.

### Requisitos Não Funcionais

* RNF01 – Código modular e seguindo padrões POO.
* RNF02 – Interface REST simples e intuitiva.
* RNF03 – Sistema deve ser rápido e responsivo.
* RNF04 – Uso de dados estruturados (JSON).
* RNF05 – Documentação clara e acessível.

---

## Modelagem UML

A seguir estão as imagens dos diagramas UML do sistema:

### **Diagrama de Classes**

![Diagrama de Classes](uml/diagrama_classes.png)

### **Diagrama de Sequência**

![Diagrama de Sequência](uml/diagrama_sequencia.png)

*(As imagens devem ser colocadas na pasta `/uml` do repositório GitHub.)*
O projeto inclui:

* Diagrama de Classes
* Diagrama de Sequência (opcional)

*(Inserir o diagrama gerado com PlantUML)*

---

## Componentes do Sistema

* **Módulo de Produtos** – cadastro e consulta.
* **Módulo de Carrinho** – controle de compras.
* **Módulo de Clientes** – gerenciamento básico.
* **Módulo de API** – integração via HTTP.

---

## Aplicação dos Conceitos de POO

Cada classe representa um elemento real do sistema e inclui:

* Propriedades privadas
* Métodos públicos de acesso
* Construtores especializados
* Organização modular em headers e fontes

---

## Trabalho Colaborativo

O projeto foi estruturado para permitir colaboração via GitHub:

* Pull Requests
* Branches por funcionalidade
* Commits descritivos
* Issues para controle de desenvolvimento

---

## Teste de Integração

Foram realizados testes entre os módulos:

* Produto x Loja
* Carrinho x Cliente
* API x Banco de Dados (ou memória)
* Respostas HTTP usando JSON

---

## Refatoração

Durante o desenvolvimento foram aplicadas melhorias:

* Melhoria de nomes de métodos
* Separação de responsabilidades
* Redução de acoplamento
* Organização em módulos independentes

---

## Relato Reflexivo

O projeto demonstrou a importância da modelagem UML antes da codificação, reforçando a clareza estrutural e o entendimento do domínio. A criação da API ampliou o potencial de uso do sistema, tornando-o consumível por outras aplicações.

---

## Como Executar

### Compilar:

```bash
g++ api.cpp -o server -std=c++17
```

### Executar:

```bash
./server
```

### Acessar API:

* `GET /produtos`
* `POST /carrinho/adicionar`
* `GET /carrinho/{id}`

---

## Referências

* Documentação oficial C++
* GitHub do httplib
* nlohmann/json – JSON for Modern C++
* Fowler, Martin – UML Essencial
* Gamma et al. – Design Patterns

---

## Anexos

* Diagramas UML
* Código-fonte completo
* Exemplos de testes
* Prints da API funcionando
