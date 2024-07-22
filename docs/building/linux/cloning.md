Obtaining the Source
====================

If you are reading this text directly on your system, you have already completed this step. Otherwise, you can obtain the source as follows (skip any steps you have already completed).


Set up access to GitHub
-----------------------

If you are reading this text directly on your system, you have already completed this step. Otherwise, you can obtain the source as follows (skip any steps you have already completed)

* Create a suitable GitHub Personal Access Token

  - Log into your GitHub account, and navigate to https://github.com/settings/tokens to generate your PAT. It's good idea to create specific tokens for each of your projects/employments, so that you can revoke it later if you wish.

* Set up GIT on your computer:

  - Open a Terminal window, and install the `git` command-line tool:

    ```shell
    $ sudo apt install git
    ```

  - Create or add to the file `$HOME/.gitconfig`:

    ```
    [user]
      name = Your Name
      email = you@email.com
    [credential]
      helper = store
    ```

    (Replace `Your Name` and `your.name` as appropriate).

  - Create or add to the file `$HOME/.git-credentials`:

    ```
    https://github-username:TOKEN@github.com
    ```

    (Replace `github-username` with your actual GitHub username, and `TOKEN` with a the contents of the Personal Access Token you created above).


* You should now be able to pull all required source code to build product from [`common-core`](https://github.com/torslettnes/common-core/) repository:

  ```shell
  git clone https://github.com/torslettnes/common-core.git
  ```


Next Steps
----------

If you haven't yet done so, refer to [Preparing Your Build Environment](preparing.md) for details on what tools you need in order to build the code in this repository.  Then, head to [Building](building.md) to get going.
