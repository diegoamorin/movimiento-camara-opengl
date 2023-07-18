#include <GL/glut.h>

#include <math.h>

const float rho = 1.0; // el radio
float angulo_theta = 3.9;
float angulo_phi = 2.2;

// Radianes a aumentar o disminuir a los angulos theta y phi
const float saltos = 0.05;
// Valor de movimiento de la camara para avanzar, retroceder, etc.
const float valor_movimiento = 1.0;

// Valores para el punto "eye"
float eye_x = 5.0, eye_y = 5.0, eye_z = 5.0;
// Valores para el punto "center" cuando el punto "eye" esta en (0, 0, 0)
float center_x = 0.0, center_y = 0.0, center_z = 0.0;

void ventana_escalable(int width, int height) {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20.0, (GLfloat)width / (GLfloat)height, 1.0, 2000.0);

    glMatrixMode(GL_MODELVIEW);
}

void escena() {
    // Función de visualización
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Posición de la cámara
    glLoadIdentity();
    gluLookAt(
        eye_x, eye_y, eye_z,
        eye_x + center_x, eye_y + center_y, eye_z + center_z,
        0.0, 1.0, 0.0
    );

    // Dibujar lineas x, y, z
    glPushMatrix();
        GLfloat line_material_diffuse[] = {1.0, 0.5, 0.0, 1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, line_material_diffuse);

        glBegin(GL_LINES);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(50.0, 0.0, 0.0);
        glEnd();
        glBegin(GL_LINES);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(0.0, 50.0, 0.0);
        glEnd();
        glBegin(GL_LINES);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(0.0, 0.0, 50.0);
        glEnd();
    glPopMatrix();

    // Dibujar tetera
    glPushMatrix();
        GLfloat material_diffuse[] = {0.6, 0.6, 1.0, 1.0};
        GLfloat material_specular[] = {1.0, 1.0, 1.0, 1.0};
        GLfloat material_shininess = 50.0;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);

        glutSolidTeapot(0.5);
    glPopMatrix();
    glFlush();
}

void luces() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat light_pos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
}

void teclado_girar_camara(int key, int x, int y) {
    float saltos = 0.05;

    switch (key) {
        case GLUT_KEY_LEFT: {
            angulo_theta -= saltos;
        } break;
        case GLUT_KEY_RIGHT: {
            angulo_theta += saltos;
        } break;
        case GLUT_KEY_UP: {
            angulo_phi -= saltos;
            // Limitar el giro hacia arriba
            float angulo_undecimal = floor(angulo_phi * 10) / 10;
            if (angulo_undecimal <= 0.0) {
                angulo_phi += saltos;
            }
        } break;
        case GLUT_KEY_DOWN: {
            angulo_phi += saltos;
            // Limitar el giro hacia abajo
            float angulo_undecimal = floor(angulo_phi * 10) / 10;
            if (angulo_undecimal >= 3.0) {
                angulo_phi -= saltos;
            }
        } break;
    }

    // De coordenada esferica (rho, theta, phi) a coordenada rectangular (x, y, z)
    center_x = rho * sin(angulo_phi) * cos(angulo_theta);
    center_z = rho * sin(angulo_phi) * sin(angulo_theta);
    center_y = rho * cos(angulo_phi);

    glutPostRedisplay();
}

void avanzar_izq_der(bool is_izq) {
    // Girar para la izq o derecha 90 grados
    if (is_izq) {
        angulo_theta -= (M_PI / 2);
    } else {
        angulo_theta += (M_PI / 2);
    }

    // Convertir coordenada esferica a coordenada rectangular,
    // no se calcula ly porque angulo_phi no cambia
    center_x = rho * sin(angulo_phi) * cos(angulo_theta);
    center_z = rho * sin(angulo_phi) * sin(angulo_theta);

    // Avanzamos hacia adelante, ignorando el valor en "y" para no
    // multiplicar valores en "y", solo movimiento a la izquierda en "x" y "z"
    eye_x = eye_x + center_x * valor_movimiento;
    eye_z = eye_z + center_z * valor_movimiento;

    // Retornamos al angulo inicial
    if (is_izq) {
        angulo_theta += (M_PI / 2);
    } else {
        angulo_theta -= (M_PI / 2);
    }
    // De coordenada esferica a coordenada rectangular
    center_x = rho * sin(angulo_phi) * cos(angulo_theta);
    center_z = rho * sin(angulo_phi) * sin(angulo_theta);
}

// Función que escucha las teclas que mueven la camara en el espacio.
void teclado_mover_camara(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': // Avanzar
            eye_x = eye_x + center_x * valor_movimiento;
            eye_z = eye_z + center_z * valor_movimiento;
            eye_y = eye_y + center_y * valor_movimiento;
            break;
        case 's': // Retroceder
            eye_x = eye_x - center_x * valor_movimiento;
            eye_z = eye_z - center_z * valor_movimiento;
            eye_y = eye_y - center_y * valor_movimiento;
            break;
        case 'a': { // Izquierda
            avanzar_izq_der(true);
        } break;
        case 'd': { // Derecha
            avanzar_izq_der(false);
        } break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    // Inicializar GLUT y crear la ventana
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Movimiento de cámara en OpenGL");

    // Activar la profundidad
    glEnable(GL_DEPTH_TEST);

    // La función ventana_escalable se llama cuando la ventana se crea inicialmente
    // y también cada vez que se redimensiona gracias a "glutReshapeFunc"
    glutReshapeFunc(ventana_escalable);

    // Configurar las luces
    luces();

    // Configurar la función de visualización
    glutDisplayFunc(escena);

    // Mover la camara con las flechas
    glutSpecialFunc(teclado_girar_camara);

    // Registrar la función de devolución de llamada para el teclado
    glutKeyboardFunc(teclado_mover_camara);

    glutMainLoop();
    return 0;
}
