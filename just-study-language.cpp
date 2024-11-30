#include <iostream>
#include "functions.h"

int main()
{
    std::cout << "My Vector!\n";

    MyVector<int> vec{1, 2, 3, 4, 5};

    printMyVector(vec);

    {
        MyVector<char> vec2{'a', 'b', 'c'};
        printMyVector(vec2);
        MyVector<int> vec3(vec);
        printMyVector(vec3);
    }

    MyVector<int> vec4(std::move(vec));
    printMyVector(vec4);

    MyVector<int> vec5(vec4);
    printMyVector(vec5);

    std::cout << vec4.size() << " " << vec4.capacity() << "\n";

    vec4.push_back(5);
    vec4.push_back(6);
    vec4.push_back(7);

    printMyVector(vec4);
    std::cout << vec4.size() << " " << vec4.capacity() << "\n";

    vec4.shrink_to_fit();

    printMyVector(vec4);
    std::cout << vec4.size() << " " << vec4.capacity() << "\n";

    vec4.clear();
    std::cout << vec4.size() << " " << vec4.capacity() << "\n";

    vec5.emplace_back(5);
    vec5.emplace_back(5);
    printMyVector(vec5);


    return  0;
}
