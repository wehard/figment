/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <willehard@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/22 19:41:00 by wkorande          #+#    #+#             */
/*   Updated: 2022/01/06 15:53:29 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "emscripten.h"

#include <stdlib.h>
#include "Application.h"
#include "GLContext.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

GLContext gl = GLContext("crap canvas", 1920, 1080);
Application app = Application(gl);

void main_loop()
{
	app.RenderFrame();
}

int main(int argc, char **argv)
{

	app.Run();
	emscripten_set_main_loop(&main_loop, 0, true);
	return (0);
}
