OUTPUT_DIR = Output

all:
	$(shell mkdir $(OUTPUT_DIR) 2>/dev/null)
	pdflatex -c-style-errors --output-directory=$(OUTPUT_DIR) Thesis.tex
	
clean:
	rm -rf $(OUTPUT_DIR)