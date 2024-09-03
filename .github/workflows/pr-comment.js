// @ts-check

// This snippet was originally from https://github.com/oprypin/nightly.link/blob/master/.github/workflows/pr-comment.yml
// but it has been modified many times since then. 

/** @param {import('github-script').AsyncFunctionArguments} AsyncFunctionArguments */
module.exports = async function ({ github, context, core }) {
  const event =
    /** @type {import('@octokit/webhooks').EmitterWebhookEvent<"workflow_run">["payload"]}  */ (
      context.payload
    );
  const workflow_run = event.workflow_run;
  try {
    async function upsertComment(
      owner,
      repo,
      issue_number,
      purpose,
      old_body,
      body
    ) {
      const { data: comments } = await github.rest.issues.listComments({
        owner,
        repo,
        issue_number,
      });

      const marker = `\r\n<!-- DO NOT REMOVE!! -->\r\n<!-- bot: ${purpose} -->\r\n`;

      const old_marker =
        new RegExp(marker.replace("\r\n", "\r?\n")).exec(old_body)?.[0] ??
        marker;

      body = (old_body ?? "").split(old_marker)[0] + marker + body;
      await github.request("PATCH /repos/{owner}/{repo}/pulls/{pull_number}", {
        owner: owner,
        repo: repo,
        pull_number: issue_number,
        body: body,
        headers: {
          "X-GitHub-Api-Version": "2022-11-28",
        },
      });
    }

    const { owner, repo } = context.repo;
    const run_id = workflow_run.id;

    const head = {
      branch: workflow_run.head_branch,
      repo: workflow_run.head_repository.name,
      sha: workflow_run.head_sha,
    };

    /**
     * @param {{branch: string | null, repo: string | null, sha: string}} head
     */
    async function findPullRequest(head) {
      for await (const { data } of github.paginate.iterator(
        github.rest.pulls.list,
        { owner, repo, head: `${head.repo}:${head.branch}`, sort: "updated" }
      ))
        for (const pull of data) if (pull.head.sha === head.sha) return pull;
    }

    // /** @type {{old_body: string | null, issue_number: number }} */
    const pull = await findPullRequest(head);
    if (pull == null) {
      core.setFailed(
        `No matching pull request found for commit sha of ${head}`
      );
      return;
    }
    const issue_number = pull.number;
    const old_body = pull.body ?? "";
    const pull_head_sha = pull.head.sha;

    core.info(`Using pull request ${issue_number}, with sha: ${pull_head_sha}`);

    let old_sha = /\<\!-- commit-sha: (?<sha>[a-z0-9]+) --\>/i.exec(old_body)
      ?.groups?.sha;
    if (old_sha != null && pull_head_sha == old_sha)
      return core.setFailed("Comment is already up-to-date!");

    const artifacts = await github.paginate(
      github.rest.actions.listWorkflowRunArtifacts,
      { owner, repo, run_id }
    );
    if (!artifacts.length) {
      return core.setFailed(
        `No artifacts found, perhaps Build Template was skipped`
      );
    }
    const template = artifacts[0];

    let body = `<!-- commit-sha: ${pull_head_sha} -->\n`;
    body += `## Download the template for this pull request: \n\n`;
    body += `> [!NOTE]  
        > This is auto generated from [\`${context.workflow}\`](${context.serverUrl}/${context.repo.owner}/${context.repo.repo}/actions/runs/${context.runId})\n`;
    body += `- via manual download: [${template.name}.zip](https://nightly.link/${owner}/${repo}/actions/artifacts/${template.id}.zip)\n`;
    body += `- via PROS Integrated Terminal: \n \`\`\`
        curl -o ${template.name}.zip https://nightly.link/${owner}/${repo}/actions/artifacts/${template.id}.zip;
        pros c fetch ${template.name}.zip;
        pros c apply ${template.name};
        rm ${template.name}.zip;
  \`\`\``;

    core.info(`Review thread message body: \n${body}`);

    await upsertComment(
      owner,
      repo,
      issue_number,
      "nightly-link",
      old_body,
      body
    );
  } catch (error) {
    core.setFailed(`Action failed with error ${error}`);
  }
}
