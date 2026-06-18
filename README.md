# Relational Algebra Lab

Лабораторная работа по реализации DSL, AST, оптимизации и backend-перевода запросов реляционной алгебры.

## Текущий статус

Реализована первая итерация:

- чистый AST реляционной алгебры;
- C++ DSL с функциональным и pipe-синтаксисом;
- visitor-принтеры для AST и RA-нотации;
- графическое demo на Dear ImGui;
- тесты AST/DSL и существующих core/stream модулей.

Реализована вторая итерация:

- отдельный слой `include/visitor`;
- оптимизатор в `include/query_optimizer`;
- правила push-down selection и projection pruning;
- отчет оптимизации и демонстрация в ImGui.

Реализована третья итерация:

- общий `QueryResult` и каталог источников;
- исполнение AST над CSV через существующие streams;
- компиляция AST в параметризованный SQL;
- исполнение SQL в SQLite;
- одинаковый end-to-end пример для CSV и SQLite во вкладке `Backends`.

Публичные заголовки разложены по предметным каталогам `include/`, а нетемплейтные реализации находятся в соответствующих каталогах `src/`. В заголовочных `*.tpp` остается только код шаблонных core/streams типов.

Подробнее: [docs/iteration1_dsl_ast.md](docs/iteration1_dsl_ast.md).
Оптимизатор: [docs/iteration2_optimizer.md](docs/iteration2_optimizer.md).
Бэкенды: [docs/iteration3_backends.md](docs/iteration3_backends.md).

## Команды

```bash
make program
make run
make tests
make lint
make demo-data
```

Ручная сборка:

```bash
cmake -S . -B build
cmake --build build --target relalg_program
./build/program
```
