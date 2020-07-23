# Contributing

## License Agreement

By contributing code to this project in any form, including sending
a pull request via Github or a code fragment or patch via private email or
public discussion groups, you agree to release your code under the terms
of the BSD license that you can find in the LICENSE.md file included in 
the source distribution.

## Code Formatting

The source code in this repository is formatted using auto-formatting tools. This is done to keep a consistent style across all source files regardless of author. Furthermore, during code reviews, this minimizes arguments about code styling between developers.

The intended work flow (using auto-formating tools) is as follows:

1. Write code.
2. Commit to git like usual.
3. **After committing to git**, run the auto format tool.
4. Run `git diff`.
5. **Review the changes from the auto formatting tool to ensure they are appropriate.**
6. If there are strange / awkward changes, revise your code and repeat steps 2-5.

### C Code Formatting

The C code within this repository (`/sdk/bare/`) is formatted using an auto-formatting tool.  The coding style closely follows the [Webkit guidelines](https://webkit.org/code-style-guidelines/), with minor modifications. The formatting style file resides in the root of this repository (`/.clang-format`). This defines the style rules.

To run the auto-formatting tool on your code, make sure you have installed version 10 or later of `clang-format`. The following scripts are designed to run on Linux. Using these on Windows or Mac will require some adjustment. 

#### Check Formatting

To simply check your code for style correctness **and not change the code**, run `scripts/check-format.sh` from the root of this repository. This script will output the required changes to adhere to style rules.

#### Format Inline

The auto-format tool can also change your code inline to adhere to the style rules. To do this, run `/scripts/format.sh` from the root directory of this repository. **This script will format your code inline. These changes cannot be undone.** After formatting, look at the changes using `git diff` to ensure they are reasonable.

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
