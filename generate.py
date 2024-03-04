line = "<p>Lorem ipsum dolor sit amet consectetur adipisicing elit. Accusamus ipsam adipisci sequi! Reiciendis</p>\n"

number_of_chunks = 10
number_of_lines_in_chunk = 10

chunk = line * number_of_lines_in_chunk

with open(f"htdocs/huge.html", "w") as file:
    file.write("<html><body>\n")
    for i in range(number_of_chunks):
        file.write(f"<p>Chunk {i + 1}</p>\n")
        file.write(chunk)
    file.write("</body></html>")
