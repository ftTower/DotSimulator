
BUILD_BONUS_DIR		=	build_bonus
SRC_DIR				=	
BUILD_DIR			=	build
NAME				=	lifegame
CFLAG				=	-Wall -Wextra -Werror -I./header/ -I./minilibx-linux/ -g3
LDFLAGS 			= 	-L./minilibx-linux -lmlx_Linux -lmlx -lX11 -lXext

SRC					=	main.c map_func.c img.c init.c dot.c dot_life.c get_set.c
						
CC					=	cc # Add this line

OBJ					=	$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o, $(SRC))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC)	$(CFLAG)	-c	$< -o	$@
	@clear

all : $(NAME)

$(NAME):$(OBJ)
	@echo $(OBJ)
	@$(CC) $(OBJ) -o $(NAME) $(CFLAG) $(LDFLAGS)
	@clear

clear :
	@clear
	@echo "42Paris : $(NAME)"
	@echo ""

clean : clear
	@rm -rf $(BUILD_DIR)
	@rm -rf $(BUILD_BONUS_DIR)
	@echo "Clean   : *.o in build and build_bonus!"

fclean : clean
	@rm -f $(NAME)
	@echo "Clean   : ./$(NAME)"

aclean : fclean cleanlib clear

minlibx :
	@git clone https://github.com/42Paris/minilibx-linux.git
	@make -C ./minilibx-linux
	clear
	
cleanlib :
	@rm -rf minilibx-linux

re: fclean all clear 

.PHONY: all clean fclean re