# Contributing

## License Agreement

By contributing code to this project in any form, including sending
a pull request via Github or a code fragment or patch via private email or
public discussion groups, you agree to release your code under the terms
of the BSD license that you can find in the LICENSE.md file included in 
the source distribution.

## Code Formatting

The C code within this repository (`/sdk/bare/`) is formatted using an auto-formatting tool. This is done to keep a consistent style across all source files regardless of author. The coding style closely follows the [Webkit guidelines](https://webkit.org/code-style-guidelines/), with minor modifications.

To run the auto-formatting tool on your code, make sure you have installed version 10 or later of `clang-format`. Then, run the `/scripts/format.sh` from the root directory of the repository. This script will format your code inline. After formatting, look at the changes using `git diff` to ensure they are reasonable.

The formatting style file resides in the root of this repository (`/.clang-format`).

## How to Use GitHub Issues

GitHub Issues is used to track development actions for this repository.
If there is a problem or concern, create an issue using the GitHub web
interface. Please ensure a duplicate issue doesn't already exist.

1. Create a descriptive title that summarizes the issue
2. Write enough details in the description such that anyone else familiar
   with the project can take action and be productive on this topic
3. Populate metadata fields:
   1. Use `Assignees` to assign relevant people to work on the issue
   2. Use `Labels` to assign appropriate tags
   3. Use `Milestone` to assign issue to group

## How to provide a patch for a new feature

Use the following procedure to submit a patch:

1. Fork the repository [on GitHub](http://help.github.com/fork-a-repo/) -- _Only for people not part of the Severson-Group_
2. Create a topic branch `git checkout -b my_branch`
3. Push to your branch `git push origin my_branch`
4. Initiate a pull request [on GitHub](https://help.github.com/articles/creating-a-pull-request/)
