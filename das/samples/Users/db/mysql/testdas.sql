CREATE DATABASE IF NOT EXISTS testdas;
USE testdas;

DELETE FROM `mysql`.`user` WHERE User = 'dastestuser';
DELETE FROM `mysql`.`db` WHERE User = 'dastestuser';
FLUSH PRIVILEGES;

CREATE USER 'dastestuser'@'localhost' IDENTIFIED BY 'dastestuserpassword';
GRANT ALL ON testdas.* TO 'dastestuser'@'localhost';

DROP TABLE IF EXISTS users_to_groups, groups, users;

SET sql_mode = 'NO_AUTO_VALUE_ON_ZERO';

CREATE TABLE `users`
(
  `id` INTEGER NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(32) NOT NULL UNIQUE,
  `password` VARCHAR(32) NOT NULL,
  `description` VARCHAR(64) NOT NULL,
  PRIMARY KEY (`id`)
)
ENGINE = InnoDB
CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE `groups`
(
  `id` INTEGER NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(32) NOT NULL UNIQUE,
  `description` VARCHAR(64) NOT NULL,
  PRIMARY KEY (`id`)
)
ENGINE = InnoDB
CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE `users_to_groups`
(
  `id` INTEGER  NOT NULL AUTO_INCREMENT,
  `userid` INTEGER  NOT NULL,
  `groupid` INTEGER NOT NULL,
  PRIMARY KEY (`id`),
  CONSTRAINT `fk_userid` FOREIGN KEY `fk_userid` (`userid`)
    REFERENCES `users` (`id`)
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_groupid` FOREIGN KEY `fk_groupid` (`groupid`)
    REFERENCES `groups` (`id`)
    ON DELETE CASCADE
    ON UPDATE CASCADE
)
ENGINE = InnoDB
CHARACTER SET utf8 COLLATE utf8_general_ci;


INSERT INTO `users`(`id`, `name`, `password`, `description`) VALUES
(0, 'admin', 'admin', 'Administrator'),
(1, 'nobody', '', 'System user'),
(2, 'guest', 'guest', 'Guest'),
(3, 'user', 'user', 'User');


INSERT INTO `groups` VALUES
(0, 'admin', 'Administrators'),
(1, 'guest', 'Guests'),
(2, 'user', 'Users');


INSERT INTO `users_to_groups` VALUES
(0, 0, 0),
(1, 2, 1),
(2, 3, 2);
