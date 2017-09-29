#!/bin/sh

sudo mysql
CREATE USER 'notka'@'localhost' IDENTIFIED BY 'password';
GRANT ALL PRIVILEGES ON * . * TO 'notka'@'localhost';
flush privileges;

mysql -u notka -p
CREATE DATABASE notka;
USE notka;
CREATE TABLE users (user VARCHAR(32) NOT NULL UNIQUE, password VARCHAR(32) NOT NULL);
CREATE TABLE notkas (user VARCHAR(32) NOT NULL UNIQUE, notka BLOB);

