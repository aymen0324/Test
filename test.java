import java.util.Scanner;

public class test {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        boolean validOption = false;

        while (!validOption) {
            // Mostrar el menú
            System.out.println("Menú:");
            System.out.println("1. Opción 1");
            System.out.println("2. Opción 2");
            System.out.println("3. Opción 3");
            System.out.println("4. Salir");
            System.out.print("Elige una opción: ");

            try {
                // Leer la opción del usuario
                int option = Integer.parseInt(scanner.nextLine());

                // Verificar si la opción es válida
                if (option >= 1 && option <= 4) {
                    System.out.println("Opción válida");
                    validOption = true; // Salir del bucle
                } else {
                    System.out.println("Opción no válida, por favor elige una opción entre 1 y 4.");
                }
            } catch (NumberFormatException e) {
                // Manejo de la excepción si el usuario ingresa algo que no es numérico
                System.out.println("Solo se permiten números.");
            }
        }

        scanner.close();
    }
}
