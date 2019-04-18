load("@bazel_tools//tools/build_defs/repo:utils.bzl", "workspace_and_buildfile")

def _clone_git_impl(ctx):
    repo_root = ctx.path(".")
    git = ctx.which("git")
    ctx.execute([git, "clone", ctx.attr.url, "."])

#    ctx.file("WORKSPACE", "workspace(name = \"{name}\")".format(name = ctx.name))

_clone_git_impl_attrs = {
    "url": attr.string(),
}

clone_git = repository_rule(
    implementation = _clone_git_impl,
    attrs = _clone_git_impl_attrs,
)

def _get_avr_toolchain_def(ctx):
    repo_root = ctx.path(".")
    avr_gcc = ctx.attr.avr_gcc
    if avr_gcc == "":
        avr_gcc = ctx.which("avr-gcc")
    ctx.file("BUILD", "compiler \"{compiler}\"".format(compiler=avr_gcc))
    print(avr_gcc)

_get_avr_toolchain_def_attrs = {
    "avr_gcc": attr.string(),
}

create_avr_toolchain = repository_rule(
    implementation = _get_avr_toolchain_def,
    attrs = _get_avr_toolchain_def_attrs,
)
    