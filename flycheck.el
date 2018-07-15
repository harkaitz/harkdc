(
flycheck-define-checker hrkc "C/C++ checker."
  :command ("hrkc" "-t" "x86_64-redhat-linux" "check" source-inplace)

			 :error-patterns
			 ((warning line-start (file-name) ":" line ":" column
			   ": warning: " (message) line-end)
			  (error line-start (file-name) ":" line ":" column
			   ": error: " (message) line-end))
		     
   :modes (c-mode c++-mode)
)
(add-to-list 'flycheck-checkers 'hrkc)