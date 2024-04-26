/opt/homebrew/opt/llvm/bin/clang --target=wasm32 -nostdlib -O3 -o wasm_module.o -c wasm_module.cpp
#/opt/homebrew/opt/llvm/bin/wasm-ld	--no-entry	--export-all	--lto-O3	--allow-undefined	--import-memory	wasm_module.o	-o wasm_module.wasm
/opt/homebrew/opt/llvm/bin/wasm-ld	--no-entry	--export-all	--lto-O3	--allow-undefined	wasm_module.o	-o wasm_module.wasm
cp wasm_module.wasm ../build/debug/.
