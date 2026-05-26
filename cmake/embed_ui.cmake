function(stickinthemd_escape_cpp_string input out_var)
  string(REPLACE "\\" "\\\\" _escaped "${input}")
  string(REPLACE "\"" "\\\"" _escaped "${_escaped}")
  string(REPLACE "\r\n" "\\n" _escaped "${_escaped}")
  string(REPLACE "\n" "\\n" _escaped "${_escaped}")
  string(REPLACE "\r" "\\n" _escaped "${_escaped}")
  set(${out_var} "${_escaped}" PARENT_SCOPE)
endfunction()

function(stickinthemd_embed_ui input_html output_cpp)
  file(READ "${input_html}" _content)
  string(LENGTH "${_content}" _len)

  set(_chunk_size 6000)
  set(_body "")
  set(_offset 0)

  while(_offset LESS _len)
    string(SUBSTRING "${_content}" ${_offset} ${_chunk_size} _chunk)
    stickinthemd_escape_cpp_string("${_chunk}" _escaped_chunk)
    string(APPEND _body "    out += \"${_escaped_chunk}\";\n")
    math(EXPR _offset "${_offset} + ${_chunk_size}")
  endwhile()

  file(WRITE "${output_cpp}"
"#include \"stickinthemd/assets.hpp\"\n\nnamespace stickinthemd {\n\nstd::string load_ui_html() {\n  std::string out;\n  out.reserve(${_len} + 256);\n${_body}  return out;\n}\n\n} // namespace stickinthemd\n"
  )
endfunction()
