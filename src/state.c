#include "state.h"
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

// Definiciones de funciones de ayuda.
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);




/* Tarea 1 */
game_state_t* create_default_state() {

  
  game_state_t* state = malloc(sizeof(game_state_t));
  if (state == NULL) {   
    return NULL;
  }

  
  const unsigned int ROWS = 18;
  const unsigned int COLS = 20;
  state->num_rows = ROWS;

  //tablero
  const char* template_board[18] = {
    "####################",
    "#                  #",
    "# d>D    *         #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "####################"
  };

  
  state->board = malloc(sizeof(char*) * ROWS);
  if (state->board == NULL) {   
    free(state);
    return NULL;
  }

  for (unsigned int r = 0; r < ROWS; ++r) {
    state->board[r] = strdup(template_board[r]);   
  }

  
  state->num_snakes = 1;
  state->snakes = malloc(sizeof(snake_t));
  if (state->snakes == NULL) {  
    
    for (unsigned int r = 0; r < ROWS; ++r) free(state->board[r]);
    free(state->board);
    free(state);
    return NULL;
  }

  
  state->snakes[0].tail_row = 2;
  state->snakes[0].tail_col = 2;
  state->snakes[0].head_row = 2;
  state->snakes[0].head_col = 4;
  state->snakes[0].live     = true;

  return state;
}



/* Tarea 2 */
void free_state(game_state_t* state) {
  if (state == NULL) return;

  
  for (unsigned int r = 0; r < state->num_rows; ++r) {
    free(state->board[r]);
  }
  free(state->board);   
  free(state->snakes);  
  free(state);          
}


/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
  if (state == NULL || fp == NULL) return;

  for (unsigned int r = 0; r < state->num_rows; ++r) {
    fputs(state->board[r], fp);
    fputc('\n', fp);          
  }
}


/**
 * Guarda el estado actual a un archivo. No modifica el objeto/struct state.
 * (ya implementada para que la utilicen)
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Tarea 4 */



/* Tarea 4.1 */






/**
 * Funcion de ayuda que obtiene un caracter del tablero dado una fila y columna
 * (ya implementado para ustedes).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}


/**
 * Funcion de ayuda que actualiza un caracter del tablero dado una fila, columna y
 * un caracter.
 * (ya implementado para ustedes).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}


/**
 * Retorna true si la variable c es parte de la cola de una snake.
 * La cola de una snake consiste de los caracteres: "wasd"
 * Retorna false de lo contrario.
*/
static bool is_tail(char c) {
  // TODO: Implementar esta funcion.
  return (c == 'w' || c == 'a' || c == 's' || c == 'd');
}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
static bool is_head(char c) {
  // TODO: Implementar esta funcion.
  return (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x');
}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implementar esta funcion.
  return is_tail(c) || c == '^' || c == '<' || c == 'v' || c == '>' || is_head(c);
}


/**
 * Convierte un caracter del cuerpo de una snake ("^<v>")
 * al caracter que correspondiente de la cola de una
 * snake ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implementar esta funcion.
  switch (c) {
    case '^': return 'w';
    case '<': return 'a';
    case 'v': return 's';
    case '>': return 'd';
    default : return '?';   /* nunca debería pasar */
  }
}


/**
 * Convierte un caracter de la cabeza de una snake ("WASD")
 * al caracter correspondiente del cuerpo de una snake
 * ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implementar esta funcion.
  switch (c) {
    case 'W': return '^';
    case 'A': return '<';
    case 'S': return 'v';
    case 'D': return '>';
    default : return '?';   /* para 'x' no se usa */
  }
}


/**
 * Retorna cur_row + 1 si la variable c es 'v', 's' o 'S'.
 * Retorna cur_row - 1 si la variable c es '^', 'w' o 'W'.
 * Retorna cur_row de lo contrario
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implementar esta funcion.
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row;
}


/**
 * Retorna cur_col + 1 si la variable c es '>' or 'd' or 'D'.
 * Retorna cur_col - 1 si la variable c es '<' or 'a' or 'A'.
 * Retorna cur_col de lo contrario
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implementar esta funcion.
  if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  return cur_col;
}


/**
 * Tarea 4.2
 *
 * Funcion de ayuda para update_state. Retorna el caracter de la celda
 * en donde la snake se va a mover (en el siguiente paso).
 *
 * Esta funcion no deberia modificar nada de state.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t* s  = &state->snakes[snum];
  char     hd = get_board_at(state, s->head_row, s->head_col);

  unsigned int nr = get_next_row(s->head_row, hd);
  unsigned int nc = get_next_col(s->head_col, hd);
  return get_board_at(state, nr, nc);
}


/**
 * Tarea 4.3
 *
 * Funcion de ayuda para update_state. Actualiza la cabeza de la snake...
 *
 * ... en el tablero: agregar un caracter donde la snake se va a mover (¿que caracter?)
 *
 * ... en la estructura del snake: actualizar el row y col de la cabeza
 *
 * Nota: esta funcion ignora la comida, paredes, y cuerpos de otras snakes
 * cuando se mueve la cabeza.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t* s  = &state->snakes[snum];
  char     hd = get_board_at(state, s->head_row, s->head_col);
  char     bd = head_to_body(hd);

  /* 1. cabeza actual pasa a ser cuerpo */
  set_board_at(state, s->head_row, s->head_col, bd);

  /* 2. nueva posición de la cabeza */
  unsigned int nr = get_next_row(s->head_row, hd);
  unsigned int nc = get_next_col(s->head_col, hd);

  set_board_at(state, nr, nc, hd);   /* misma dirección (WASD) */
  s->head_row = nr;
  s->head_col = nc;
}

  
  


/**
 * Tarea 4.4
 *
 * Funcion de ayuda para update_state. Actualiza la cola de la snake...
 *
 * ... en el tablero: colocar un caracter blanco (spacio) donde se encuentra
 * la cola actualmente, y cambiar la nueva cola de un caracter de cuerpo (^<v>)
 * a un caracter de cola (wasd)
 *
 * ...en la estructura snake: actualizar el row y col de la cola
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  snake_t* s     = &state->snakes[snum];
  char     tailc = get_board_at(state, s->tail_row, s->tail_col);

  /* 1. borrar cola actual del tablero */
  set_board_at(state, s->tail_row, s->tail_col, ' ');

  /* 2. siguiente segmento pasa a ser la nueva cola */
  unsigned int nr = get_next_row(s->tail_row, tailc);
  unsigned int nc = get_next_col(s->tail_col, tailc);
  char bodyc      = get_board_at(state, nr, nc);

  set_board_at(state, nr, nc, body_to_tail(bodyc));
  s->tail_row = nr;
  s->tail_col = nc;
}
  

/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {

  for (unsigned int i = 0; i < state->num_snakes; ++i) {

    snake_t* s = &state->snakes[i];
    if (!s->live) continue;                

    char nxt = next_square(state, i);

    
    if (nxt == '#' || is_snake(nxt)) {
      set_board_at(state, s->head_row, s->head_col, 'x');
      s->live = false;
      continue;
    }

    /* fruta -> crece */
    if (nxt == '*') {
      update_head(state, i);                /* solo mueve cabeza */
      if (add_food != NULL) add_food(state);
      continue;
    }

    /* espacio vacío */
    if (nxt == ' ') {
      update_head(state, i);
      update_tail(state, i);
      continue;
    }

    
    set_board_at(state, s->head_row, s->head_col, 'x');
    s->live = false;
  }
}

/* Tarea 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implementar esta funcion.
  if (filename == NULL) return NULL;

  FILE* fp = fopen(filename, "r");
  if (fp == NULL) return NULL;          

  
  char*   line     = NULL;
  size_t  cap      = 0;
  ssize_t nread;

  /* reserva dinámica para filas */
  unsigned int rows_cap   = 16;                   
  unsigned int num_rows   = 0;
  char**       board_rows = malloc(sizeof(char*) * rows_cap);
  if (board_rows == NULL) { fclose(fp); return NULL; }

  while ( (nread = getline(&line, &cap, fp)) != -1 ) {

    
    if (nread > 0 && line[nread - 1] == '\n') {
      line[nread - 1] = '\0';
    }

    
    if (num_rows == rows_cap) {
      rows_cap *= 2;
      char** tmp = realloc(board_rows, sizeof(char*) * rows_cap);
      if (tmp == NULL) {        
        free(line);
        fclose(fp);
        
        for (unsigned int r = 0; r < num_rows; ++r) free(board_rows[r]);
        free(board_rows);
        return NULL;
      }
      board_rows = tmp;
    }

    
    board_rows[num_rows] = strdup(line);
    if (board_rows[num_rows] == NULL) { 
      free(line);
      fclose(fp);
      for (unsigned int r = 0; r < num_rows; ++r) free(board_rows[r]);
      free(board_rows);
      return NULL;
    }
    ++num_rows;
  }

  free(line);
  fclose(fp);

  
  game_state_t* state = malloc(sizeof(game_state_t));
  if (state == NULL) {                 
    for (unsigned int r = 0; r < num_rows; ++r) free(board_rows[r]);
    free(board_rows);
    return NULL;
  }

  state->num_rows   = num_rows;
  state->board      = board_rows;
  state->num_snakes = 0;     
  state->snakes     = NULL;

  return state;
}


/**
 * Tarea 6.1
 *
 * Funcion de ayuda para initialize_snakes.
 * Dada una structura de snake con los datos de cola row y col ya colocados,
 * atravezar el tablero para encontrar el row y col de la cabeza de la snake,
 * y colocar esta informacion en la estructura de la snake correspondiente
 * dada por la variable (snum)
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
 

  snake_t* s = &state->snakes[snum];

  
  unsigned int r = s->tail_row;
  unsigned int c = s->tail_col;
  char ch        = get_board_at(state, r, c);

  
  while (!is_head(ch)) {
    unsigned int nr = get_next_row(r, ch);
    unsigned int nc = get_next_col(c, ch);

    r  = nr;
    c  = nc;
    ch = get_board_at(state, r, c);
  }

  
  s->head_row = r;
  s->head_col = c;
  s->live     = (ch != 'x');
}

/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implementar esta funcion.
  if (state == NULL) return NULL;

  /*  contar cuántas colas hay  */
  unsigned int tails = 0;
  for (unsigned int r = 0; r < state->num_rows; ++r) {
    for (unsigned int c = 0; state->board[r][c] != '\0'; ++c) {
      if (is_tail(state->board[r][c])) {
        ++tails;
      }
    }
  }

  
  if (tails == 0) {
    state->num_snakes = 0;
    state->snakes     = NULL;
    return state;
  }

  
  state->snakes     = malloc(sizeof(snake_t) * tails);
  state->num_snakes = tails;

  
  unsigned int idx = 0;
  for (unsigned int r = 0; r < state->num_rows; ++r) {
    for (unsigned int c = 0; state->board[r][c] != '\0'; ++c) {
      if (is_tail(state->board[r][c])) {

        
        state->snakes[idx].tail_row = r;
        state->snakes[idx].tail_col = c;

        
        state->snakes[idx].head_row = r;
        state->snakes[idx].head_col = c;
        state->snakes[idx].live     = true;

        ++idx;
      }
    }
  }

  
  for (unsigned int i = 0; i < state->num_snakes; ++i) {
    find_head(state, i);
  }

  return state;
}



int main(int argc, char* argv[]) {

  
  char *infile = NULL, *outfile = NULL;

  for (int i = 1; i < argc - 1; ++i) {
    if (strcmp(argv[i], "-i") == 0) {
      infile = argv[i + 1];
    } else if (strcmp(argv[i], "-o") == 0) {
      outfile = argv[i + 1];
    }
  }

  if (infile == NULL || outfile == NULL) {
    print_usage(argv[0]);
    return -1;
  }

  /* ---- carga del tablero ---- */
  game_state_t* state = load_board(infile);
  if (state == NULL) {               
    fprintf(stderr, "Error: no se pudo leer el archivo %s\n", infile);
    return -1;
  }

  
  initialize_snakes(state);

  
  update_state(state, deterministic_food);

  
  save_board(state, outfile);

 
  free_state(state);

  return 0;
}

