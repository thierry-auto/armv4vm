git filter-branch -f --env-filter '


#autoreversed <49288366+autoreversed@users.noreply.github.com>

#WRONG_EMAIL="autoreversed@mailbox.org"
WRONG_EMAIL="49288366+autoreversed@users.noreply.github.com"

NEW_NAME="thierry-auto"
NEW_EMAIL="thierry.auto@mailbox.org"



if [ "$GIT_COMMITTER_EMAIL" = "$WRONG_EMAIL" ]
then
    export GIT_COMMITTER_NAME="$NEW_NAME"
    export GIT_COMMITTER_EMAIL="$NEW_EMAIL"
fi
if [ "$GIT_AUTHOR_EMAIL" = "$WRONG_EMAIL" ]
then
    export GIT_AUTHOR_NAME="$NEW_NAME"
    export GIT_AUTHOR_EMAIL="$NEW_EMAIL"
fi
' --tag-name-filter cat -- --branches --tags
