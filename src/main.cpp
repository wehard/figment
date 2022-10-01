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
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdlib.h>
#include "Application.h"
#include "GLContext.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

int main(int argc, char **argv)
{
	GLContext gl = GLContext("figment canvas", 1920, 1080);

	Application app = Application(gl);
	app.Run();
	return (0);
}
