# GitHub Actions troubleshooting (StuckInTheMD)

## Quick checks

```bash
gh auth status
gh run list --repo MrChriZ/StuckInTheMD --workflow ci.yml --limit 5
gh api repos/MrChriZ/StuckInTheMD/actions/permissions --jq '.enabled'
```

- Repo **Settings → Actions → General**: Actions enabled, “Allow all actions”.
- [githubstatus.com](https://www.githubstatus.com) — if **Actions** is degraded, new runs may stay **queued** with **0 jobs** or `workflow_dispatch` may return **HTTP 500**. Wait and retry.

## Symptom: push to `main` but no new run in Actions tab

1. Confirm remote HEAD: `gh api repos/MrChriZ/StuckInTheMD/commits/main --jq .sha`
2. Check [Actions](https://github.com/MrChriZ/StuckInTheMD/actions) — no run usually means the **push webhook** did not start a workflow (platform issue), not a YAML error.
3. Retrigger:
   ```bash
   git commit --allow-empty -m "ci: retrigger"
   git push origin main
   ```
4. Or **Actions → CI → Run workflow** (branch `main`) after `workflow_dispatch` is on `ci.yml`.
5. Or re-run the latest failed run: `gh run rerun <run-id> --repo MrChriZ/StuckInTheMD`

## Symptom: run stuck `queued` with no jobs

Often coincides with GitHub Actions incidents. Cancel and retry later:

```bash
gh run cancel <run-id> --repo MrChriZ/StuckInTheMD
```

## Symptom: checkout fails with “Your account is suspended”

Usually a **transient GitHub error** during an incident (not a real suspension). Verify:

```bash
gh api user --jq '{login, suspended_at}'
```

If `suspended_at` is `null` and you can push releases, re-run the workflow when Actions is operational.

## Symptom: `workflow_dispatch` HTTP 500

Known during Actions outages. Use push retrigger or wait for [GitHub Status](https://www.githubstatus.com) to clear.

## Local substitute when CI is down

```powershell
.\scripts\run-tests.ps1
.\scripts\build-windows-installer.ps1
```
