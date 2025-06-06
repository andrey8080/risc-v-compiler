#!/bin/bash

set -e

# Проверяем наличие компилятора
if [ ! -f "build/bin/compiler" ]; then
    echo "Компилятор не найден. Выполните сборку:"
    echo "   mkdir build && cd build && cmake .. && make"
    exit 1
fi

echo "Найден компилятор: build/bin/compiler"
echo ""

# Создаем демонстрационные примеры
cat > demo_variables.prog << 'EOF'
// Демонстрация переменных и арифметики
int x;
int y;
int result;

x = 10;
y = 5;
result = x + y * 2;
print result;
EOF

cat > demo_conditionals.prog << 'EOF'
// Демонстрация условных операторов
int age;
age = 25;

if (age >= 18) {
    print 1;  // Совершеннолетний
} else {
    print 0;  // Несовершеннолетний
}
EOF

cat > demo_loops.prog << 'EOF'
// Демонстрация циклов
int i;
int sum;

i = 1;
sum = 0;

while (i <= 5) {
    sum = sum + i;
    i = i + 1;
}

print sum;  // Должно вывести 15
EOF

# Демонстрируем компиляцию каждого примера
examples=("demo_variables.prog" "demo_conditionals.prog" "demo_loops.prog")
descriptions=(
    "Переменные и арифметические операции"
    "Условные операторы (if-else)"
    "Циклы (while)"
)

for i in "${!examples[@]}"; do
    example="${examples[$i]}"
    description="${descriptions[$i]}"
    
    echo "=== Пример $((i+1)): $description ==="
    echo "Исходный код ($example):"
    echo "---"
    cat "$example"
    echo "---"
    echo ""
    
    # Компилируем
    output_file="${example%.prog}.s"
    echo "Компилируем: ./build/bin/compiler $example $output_file"
    
    if ./build/bin/compiler "$example" "$output_file"; then
        echo "Компиляция успешна!"
        echo "Сгенерированный ассемблерный код ($output_file):"
        echo "---"
        head -20 "$output_file"
        if [ $(wc -l < "$output_file") -gt 20 ]; then
            echo "... (показаны первые 20 строк из $(wc -l < "$output_file"))"
        fi
        echo "---"
    else
        echo "Ошибка компиляции"
    fi
    
    echo ""
    echo "Нажмите Enter для продолжения..."
    read
done

# Запуск всех тестов
echo "=== Запуск полного набора тестов ==="
echo "В проекте есть $(ls tests/*.prog | wc -l) тестовых файлов"
echo "Запускаем все тесты..."
echo ""

if ./run_tests.sh; then
    echo "Демонстрация завершена успешно!"
else
    echo "Некоторые тесты не прошли"
    exit 1
fi

# Очистка демонстрационных файлов
echo "Очищаем демонстрационные файлы..."
rm -f demo_*.prog demo_*.s

echo "Выход из демонстрации"
