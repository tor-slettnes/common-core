Obtaining the Source
====================

If you are reading this text directly on your system, you have already completed this step. Otherwise, you can obtain the source as follows (skip any steps you have already completed).


Set up access to GitHub
-----------------------

If you are reading this text directly on your system, you have already completed this step. Otherwise, you can obtain the source as follows (skip any steps you have already completed):

* Create a suitable GitHub Personal Access Token

  - Log into your GitHub account, and navigate to https://github.com/settings/tokens to generate your PAT. It's good idea to create specific tokens for each of your projects/employments, so that you can revoke it later if you wish.

* Set up GIT on your computer:

  - Open a Terminal window, and install the `git` command-line tool:

    ```shell
    sudo apt install git
    ```

  - Create or add to the file `$HOME/.gitconfig`:

    ```ini
    [user]
      name = Your Name
      email = you@email.com
    [credential]
      helper = store
    ```

    (Replace `Your Name` and `you@email.com` as appropriate).

  - Create or add to the file `$HOME/.git-credentials`:

    ```
    https://github-username:TOKEN@github.com
    ```

    (Replace `github-username` with your actual GitHub username, and `TOKEN` with a the contents of the Personal Access Token you created above).


Clone the repository to your local workspace
--------------------------------------------

### As a submodule within another repository

This `common-core` repository is mainly intended for use as a general-purpose submodule within more targeted code bases.  As such you most likely want to clone that outer repository with the `--recurse-submodules` option, i.e.:

  ```bash
  git clone --recurse-submodules https://github.com/YOUR/REPOSITORY.git
  ```

This should bering in all submodule dependencies, including `common-core`.


### As a standalone repository

If you want to use this `common-core` repository in a standalone workspace (e.g. to explore or experiment with the shared components such as the DEMO example), simply type:

  ```bash
  git clone https://github.com/torslettnes/common-core.git
  ```


Next Steps
----------

If you haven't yet done so, refer to [Preparing Your Build Environment](preparing.md) for details on what tools you need in order to build the code in this repository.  Then, head to [Building](building.md) to get going.
