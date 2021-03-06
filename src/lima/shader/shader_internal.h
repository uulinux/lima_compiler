/* Author(s):
 *   Connor Abbott
 *
 * Copyright (c) 2014 Connor Abbott (connor@abbott.cx)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shader.h"
#include "program.h"
#include "symbols/symbols.h"
#include "glsl_parser_extras.h"
#include "main/hash_table.h"
#include "ralloc.h"
#include "pp_hir/pp_hir.h"
#include "pp_lir/pp_lir.h"
#include "gp_ir/gp_ir.h"

struct lima_shader_s
{
	void* mem_ctx;
	
	lima_shader_stage_e stage;
	lima_core_e core;
	
	struct gl_context mesa_ctx;
	_mesa_glsl_parse_state* state;
	
	struct hash_table* glsl_symbols;
	
	struct gl_shader* shader, *linked_shader;
	struct gl_shader_program* whole_program;
	
	lima_shader_symbols_t symbols;
	
	char* info_log;
	
	union {
		struct {
			lima_pp_hir_prog_t* hir_prog;
			lima_pp_lir_prog_t* lir_prog;
		} pp;
		struct {
			lima_gp_ir_prog_t* gp_prog;
		} gp;
	} ir;
	
	void* code;
	unsigned code_size;
	
	lima_shader_info_t info;
	
	bool parsed; /* whether the shader was parsed without any errors */
	bool compiled; /* whether the shader was lowered to assembly without any errors */
	bool errors;
};

void lima_convert_symbols(lima_shader_t* shader);
void lima_lower_to_pp_hir(lima_shader_t* shader);
void lima_lower_to_gp_ir(lima_shader_t* shader);

extern "C" void fill_fs_info(lima_pp_hir_prog_t* prog, lima_shader_info_t* info);
