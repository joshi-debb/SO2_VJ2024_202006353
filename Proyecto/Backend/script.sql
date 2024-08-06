CREATE DATABASE ProyectoG1;

CREATE TABLE ProyectoG1.dashboard(
    id INTEGER NOT NULL AUTO_INCREMENT,
	pid INT NOT NULL,
    name VARCHAR(255) NOT NULL,
    size INT NOT NULL,
    memoria  VARCHAR(255) NOT NULL,
    PRIMARY KEY(id)
);

CREATE TABLE ProyectoG1.llamada(
	id INTEGER NOT NULL AUTO_INCREMENT,
    pid INT NOT NULL,
    nombre VARCHAR(255) NOT NULL,
	size INT NOT NULL,
	fecha_hora DATETIME,
    PRIMARY KEY(id)
);

USE ProyectoG1;
DELIMITER //
CREATE PROCEDURE InsertOrUpdateDashboard(
    IN p_pid INT,
    IN p_name VARCHAR(255),
    IN p_size INT,
    IN p_memoria  VARCHAR(255),
    IN p_nombre  VARCHAR(255),
    IN p_time VARCHAR(255)
)
BEGIN
	INSERT INTO llamada(pid,nombre,size,fecha_hora) VALUES(p_pid,p_nombre,p_size,p_time);
    IF EXISTS (SELECT * FROM dashboard WHERE name = p_name ) THEN
        UPDATE dashboard
        SET size = p_size,
			pid = p_pid,
            memoria = p_memoria
        WHERE name =  p_name;
    ELSE
        INSERT INTO dashboard (pid, name, size, memoria)
        VALUES (p_pid, p_name, p_size, p_memoria);
        
    END IF;
END //
DELIMITER ;