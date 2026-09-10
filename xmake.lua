add_rules("mode.debug", "mode.release")

target("trabalho-U1-edb2")
    set_kind("static")
    add_files("common.c")

target("aes")
    set_kind("binary")
    add_files("aes.c")

    add_deps("trabalho-U1-edb2")

target("rsa")
    set_kind("binary")
    add_files("rsa.c")

    add_deps("trabalho-U1-edb2")

    if is_plat("macosx") then
        add_includedirs("/opt/homebrew/include")
        add_linkdirs("/opt/homebrew/lib")
        add_links("gmp")
    end
