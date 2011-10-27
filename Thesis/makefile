OUTPUT_DIR = Output


# Build the LaTeX document.
all: outputdir pdf
	
# Remove output directory.
clean:
	rm -rf $(OUTPUT_DIR)



# Create LaTeX output directory.
outputdir:
	$(shell mkdir $(OUTPUT_DIR) 2>/dev/null)
	
# Generate PDF output from LaTeX input files.
pdf:
	pdflatex -c-style-errors --output-directory=$(OUTPUT_DIR) Thesis.tex
	pdflatex -quiet --output-directory=$(OUTPUT_DIR) Thesis.tex