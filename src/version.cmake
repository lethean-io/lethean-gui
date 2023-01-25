function (write_static_version_header hash)
  set(VERSIONTAG "${hash}")
  configure_file("src/version.h.in" "version/version.h")
  add_custom_target(version ALL)
endfunction ()

