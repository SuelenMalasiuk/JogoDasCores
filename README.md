* Descrição
Jogo em que o usuário tem como objetivo excluir os retangulos com cliques do mouse, no desenvolvimento deste projeto foi utilizada a linguagem C++.

* Funcionalidades
- Grade 8x8 de retângulos coloridos com cores aleatórias.

- Clique para remover retângulos com cor semelhante.

- Pontuação baseada no número de retângulos removidos.

- Reiniciar o jogo pressionando R.

- Encerramento automático ao completar o jogo.

* Como rodar
Requisitos
Compilador C++ (ex: g++, clang)
Biblioteca GLFW
Biblioteca GLAD (para carregar funções OpenGL)
CMake (opcional, para build automatizado)
Sistema com suporte OpenGL 3.3+
Build

* Se usar CMake: *
bash
Copiar
Editar
mkdir build
cd build
cmake -G "MinGW Makefiles" .. 
make
./JogoDasCores

* Controles
Clique esquerdo: Remove retângulos de cor similar ao clicado.
Tecla R: Reinicia o jogo.
Fechar janela: Sai do jogo.
