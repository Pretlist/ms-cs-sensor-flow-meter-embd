#!/usr/bin/env python3
import os
import sys

import semver
from github import Github


class TermColors:
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"


def patched_main():
    if len(sys.argv) != 3:
        print("Usage: version_check <version>")

    # or using an access token
    if os.environ.get("GithubUserAccessToken") is not None:
        g = Github(os.environ.get("GithubUserAccessToken"))
        user = g.get_user("Pretlist")
        repo_name = sys.argv[1]
        installed_version = sys.argv[2]
        repos = user.get_repo(repo_name)
        release = repos.get_latest_release()
        print(
            f"{TermColors.OKGREEN}Info: Installed version of "
            + repo_name
            + " found "
            + installed_version
            + f"{TermColors.ENDC}"
        )
        if semver.compare(installed_version, release.tag_name):
            print(
                f"{TermColors.WARNING}Info: Latest version of "
                + repo_name
                + " found "
                + release.tag_name
                + f"{TermColors.ENDC}"
            )
            sys.exit(1)
        else:
            print(
                f"{TermColors.OKGREEN}Info: Latest version of "
                + repo_name
                + " found "
                + release.tag_name
                + f"{TermColors.ENDC}"
            )
            sys.exit(0)
    else:
        print(
            f'{TermColors.FAIL}Error: Please set ENV variable "GithubUserAccessToken"\
            {TermColors.ENDC}'
        )
        sys.exit(-1)


if __name__ == "__main__":
    patched_main()
