clc;
clear;
% Crear figura
figure;
hold on;
axis equal;
% --- Ajustar límites para mostrar toda la distribución ---
xlim([-5 50]); % Ajuste de límites X para cubrir el Rombo Gym
ylim([-10 40]); % <--- CAMBIO AQUÍ: ampliado para mostrar completamente el mapa inferior

% --- Crear mapa de calor primero (para que no se tape) ---
[xGrid, yGrid] = meshgrid(linspace(-5, 25, 300), linspace(-5, 45, 300)); % Ajuste de límites Y

% Posiciones de los rombos (Access Points)
rombos = [17.5, 22;   % Rombo superior
          17.5, 31];  % <--- ¡CAMBIO CLAVE AQUÍ! Rombo inferior (ahora en Y=31)

% --- Dibujar contenedores y rombos encima del heatmap ---
rectangle('Position', [-16, -13, 34, 49], 'EdgeColor', 'k', 'LineWidth', 1, 'FaceColor', 'none');

% Dimensiones del rombo
ancho_rombo = 1.0;
alto_rombo = 1.5;

% --- Rombo Superior ---
cx_superior = 17.5;
cy_superior = 22;
x_rombo1 = [cx_superior + ancho_rombo/2, cx_superior, cx_superior - ancho_rombo/2, cx_superior, cx_superior + ancho_rombo/2];
y_rombo1 = [cy_superior, cy_superior + alto_rombo/2, cy_superior, cy_superior - alto_rombo/2, cy_superior];
plot(x_rombo1, y_rombo1, 'k', 'LineWidth', 1);

% --- Rombo Inferior ---
cx_inferior = 17.5;
cy_inferior = -5;
x_rombo2 = [cx_inferior + ancho_rombo/2, cx_inferior, cx_inferior - ancho_rombo/2, cx_inferior, cx_inferior + ancho_rombo/2];
y_rombo2 = [cy_inferior, cy_inferior + alto_rombo/2, cy_inferior, cy_inferior - alto_rombo/2, cy_inferior];
plot(x_rombo2, y_rombo2, 'k', 'LineWidth', 1);

% --- Rectángulos adicionales ---
rectangle('Position', [22, 19, 26, 17], 'EdgeColor', 'k', 'LineWidth', 1, 'FaceColor', 'none');
rectangle('Position', [22, -7.2, 18, 26], 'EdgeColor', 'k', 'LineWidth', 5, 'FaceColor', 'none');

% --- Rombo Gym (se mantiene en 35, 31) ---
cx_gym = 35;
cy_gym = 31;
x_rombo_gym = [cx_gym + ancho_rombo/2, cx_gym, cx_gym - ancho_rombo/2, cx_gym, cx_gym + ancho_rombo/2];
y_rombo_gym = [cy_gym, cy_gym + alto_rombo/2, cy_gym, cy_gym - alto_rombo/2, cy_gym];
plot(x_rombo_gym, y_rombo_gym, 'k', 'LineWidth', 1);

% --- Crear nuevo mapa de calor (el que prevalece) ---
[xGrid, yGrid] = meshgrid(linspace(-4, 45, 800), linspace(-4, 45, 800)); % Ajuste de límites

% Posiciones de los rombos (Access Points)  
rombos = [17.5, 22;   % Rombo superior
          17.5, 0;    % <--- CAMBIO AQUÍ: Rombo inferior más abajo, visible completo
          35, 31];    % Rombo Gym (se mantiene)

for i = 1:size(rombos,1)
    cx = rombos(i,1);
    cy = rombos(i,2);
    sigma_x = 5; 
    sigma_y = 5;
    Z = exp(-(((xGrid - cx).^2) / (2*sigma_x^2) + ((yGrid - cy).^2) / (2*sigma_y^2)));
    h = imagesc(linspace(-7, 47, 800), linspace(-7, 47, 800), Z); % Ajuste de límites
    colormap(jet);
    set(h, 'AlphaData', Z*3); % Transparencia según intensidad
end

%------------------------------------------------
% --- Rectángulos y Puntos de las Duchas (se mantienen) ---
rectangle('Position', [22, 15, 6, 4], 'EdgeColor', 'k', 'LineWidth', 2, 'FaceColor', 'none');
rectangle('Position', [22, 11, 6, 4], 'EdgeColor', 'k', 'LineWidth', 2, 'FaceColor', 'none');
rectangle('Position', [22, 7, 6, 4], 'EdgeColor', 'k', 'LineWidth', 2, 'FaceColor', 'none');
rectangle('Position', [22, 3, 6, 4], 'EdgeColor', 'k', 'LineWidth', 2, 'FaceColor', 'none');
rectangle('Position', [33.5, 15, 6, 4], 'EdgeColor', 'k', 'LineWidth', 2, 'FaceColor', 'none');
rectangle('Position', [33.5, 11, 6, 4], 'EdgeColor', 'k', 'LineWidth', 2, 'FaceColor', 'none');
rectangle('Position', [33.5, 7, 6, 4], 'EdgeColor', 'k', 'LineWidth', 2, 'FaceColor', 'none');
rectangle('Position', [33.5, 3, 6, 4], 'EdgeColor', 'k', 'LineWidth', 2, 'FaceColor', 'none');

% --- Puntos Rojos (se mantienen) ---
x_punto = 24; y_punto = 16.8; plot(x_punto, y_punto, 'ro', 'MarkerSize', 5, 'MarkerFaceColor', 'r');
x_punto = 24; y_punto = 12.8; plot(x_punto, y_punto, 'ro', 'MarkerSize', 5, 'MarkerFaceColor', 'r');
x_punto = 24; y_punto = 8.8; plot(x_punto, y_punto, 'ro', 'MarkerSize', 5, 'MarkerFaceColor', 'r');
x_punto = 24; y_punto = 4.8; plot(x_punto, y_punto, 'ro', 'MarkerSize', 5, 'MarkerFaceColor', 'r');
x_punto = 37.8; y_punto = 16.8; plot(x_punto, y_punto, 'ro', 'MarkerSize', 5, 'MarkerFaceColor', 'r');
x_punto = 37.8; y_punto = 12.8; plot(x_punto, y_punto, 'ro', 'MarkerSize', 5, 'MarkerFaceColor', 'r');
x_punto = 37.8; y_punto = 8.8; plot(x_punto, y_punto, 'ro', 'MarkerSize', 5, 'MarkerFaceColor', 'r');
x_punto = 37.8; y_punto = 4.8; plot(x_punto, y_punto, 'ro', 'MarkerSize', 5, 'MarkerFaceColor', 'r');

% --- Configuración final ---
title('HeatMAP Duchas Gym ICESI', 'FontSize', 14, 'FontWeight', 'bold');
grid off;
set(gca, 'XTick', [], 'YTick', []);
axis off;
hold off;
