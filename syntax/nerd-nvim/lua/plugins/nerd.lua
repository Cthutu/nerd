return {
  {
    "neovim/nvim-lspconfig",
    init = function()
      vim.filetype.add({
        extension = {
          n = "nerd",
        },
      })
    end,
    opts = {
      servers = {
        nerd = {},
      },
      setup = {
        nerd = function(_, opts)
          local lspconfig = require("lspconfig")
          local configs = require("lspconfig.configs")

          if not configs.nerd then
            configs.nerd = {
              default_config = {
                cmd = { "nerd", "lsp" },
                filetypes = { "nerd" },
                root_dir = function(fname)
                  return lspconfig.util.root_pattern(".git")(fname)
                    or lspconfig.util.path.dirname(fname)
                end,
                single_file_support = true,
              },
            }
          end

          lspconfig.nerd.setup(opts)
          return true
        end,
      },
    },
  },
  {
    "stevearc/conform.nvim",
    opts = function(_, opts)
      opts.formatters_by_ft = opts.formatters_by_ft or {}
      opts.formatters = opts.formatters or {}

      opts.formatters_by_ft.nerd = { "nerd" }
      opts.formatters.nerd = {
        command = "nerd",
        args = { "format", "--stdout", "$FILENAME" },
        stdin = true,
      }
    end,
  },
}
