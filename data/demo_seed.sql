DROP TABLE IF EXISTS users;
DROP TABLE IF EXISTS orders;

CREATE TABLE users (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    age INTEGER NOT NULL,
    city TEXT NOT NULL
);

CREATE TABLE orders (
    id INTEGER PRIMARY KEY,
    user_id INTEGER NOT NULL,
    total REAL NOT NULL,
    status TEXT NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

INSERT INTO users (id, name, age, city) VALUES
    (1, 'Alice', 31, 'Moscow'),
    (2, 'Bob', 19, 'Kazan'),
    (3, 'Carol', 27, 'Perm'),
    (4, 'Dan', 42, 'Moscow');

INSERT INTO orders (id, user_id, total, status) VALUES
    (101, 1, 120.50, 'paid'),
    (102, 1, 45.00, 'new'),
    (103, 2, 89.90, 'paid'),
    (104, 3, 210.00, 'paid'),
    (105, 4, 15.75, 'cancelled');
